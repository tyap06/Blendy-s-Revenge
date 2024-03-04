// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include <iostream>
// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const size_t MAX_MINIONS = 800;
const size_t MINION_DELAY_MS = 200 * 3;
const float LIGHT_SOURCE_MOVEMENT_DISTANCE = 50.0f;

// UI
const vec3 BLENDY_COLOR = { 0.78f, 0.39f, 0.62f };

// DEFAULT START POSITIONS
const vec2 TOP_LEFT_OF_SCREEN = { 0.f,0.f };
const vec2 CENTER_OF_SCREEN = { window_width_px / 2, window_height_px / 2 };
const vec2 BOTTOM_RIGHT_OF_SCREEN = { window_width_px, window_height_px };
const vec2 BOTTOM_LEFT_OF_SCREEN = { 0, window_height_px };
const vec2 BOTTOM_RIGHT_OF_SCREEN_DIRECTIONAL_LIGHT	 = { window_width_px - DIRECTIONAL_LIGHT_BB_WIDTH / 2, window_height_px - DIRECTIONAL_LIGHT_BB_HEIGHT / 2};
const vec2 BLENDY_START_POSITION = { window_width_px / 2, window_height_px - 200 };

// BOUNDS
const vec2 BLENDY_BOUNDS = { BLENDY_BB_WIDTH, BLENDY_BB_HEIGHT };
const vec2 DIRECTIONAL_LIGHT_BOUNDS = { DIRECTIONAL_LIGHT_BB_WIDTH, DIRECTIONAL_LIGHT_BB_HEIGHT };
const vec2 BACKGROUND_BOUNDS = { BACKGROUND_BB_WIDTH, BACKGROUND_BB_HEIGHT };
const vec2 MINION_BOUNDS = { MINION_BB_WIDTH, MINION_BB_HEIGHT };
bool is_dead = false;
const vec2 dead_velocity = { 0, 200.0f };
const float dead_angle = 3.0f;
const vec2 dead_scale = { 0, 0 };

// EYE POSITION (For Lighting Purposes)
const float CAMERA_Z_DEPTH = 1500.f;
const vec3 CAMERA_POSITION = {window_width_px / 2, window_height_px / 2, CAMERA_Z_DEPTH};

// FPS COUNTER
const vec2 FPS_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN = { 0.f, 0.f};
const vec2 FPS_COUNTER_SCALE = { 1.f,1.f };
const vec3 FPS_TEXT_COLOR = BLENDY_COLOR;

// Create the bug world
WorldSystem::WorldSystem()
	: points(0)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (dead_sound != nullptr)
		Mix_FreeChunk(dead_sound);
	if (get_point != nullptr)
		Mix_FreeChunk(get_point);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Blendy's Revenge", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	dead_sound = Mix_LoadWAV(audio_path("dead_effect.wav").c_str());
	get_point = Mix_LoadWAV(audio_path("get_point.wav").c_str());

	if (background_music == nullptr || dead_sound == nullptr || get_point == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("dead_effect.wav").c_str(),
			audio_path("get_point.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

void WorldSystem::update_minions(float elapsed_ms_since_last_update)
{
	next_minion_spawn -= elapsed_ms_since_last_update * current_speed;

	if (registry.minions.components.size() <= MAX_MINIONS && next_minion_spawn < 0.f) {
		next_minion_spawn = (MINION_DELAY_MS / 2) + uniform_dist(rng) * (MINION_DELAY_MS / 2);

		create_minion(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), 0.0f), MINION_BOUNDS);
	}
}

// Update our game world

bool WorldSystem::step(float elapsed_ms_since_last_update) {
	
	update_fps(elapsed_ms_since_last_update);

	update_player_movement();
	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	//Main LOOP
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) 
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	if (is_dead) {
		Motion& player_motion = registry.motions.get(player_blendy);
		float sec_passed = elapsed_ms_since_last_update / 1000;
		player_motion.velocity = player_motion.velocity*(1 - sec_passed) + dead_velocity * sec_passed;
		player_motion.angle = player_motion.angle * (1 - sec_passed) + dead_angle * sec_passed;
		player_motion.scale= player_motion.scale * (1 - sec_passed) + dead_scale * sec_passed;
	}

	update_minions(elapsed_ms_since_last_update);

	// Processing the blendy state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		
		//
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	is_dead = false;
	registry.is_dead = false;
	game_background = create_background(renderer, CENTER_OF_SCREEN, BACKGROUND_BOUNDS);
	player_blendy = create_blendy(renderer, BLENDY_START_POSITION, BLENDY_BOUNDS);
	directional_light = create_directional_light(renderer, BOTTOM_RIGHT_OF_SCREEN_DIRECTIONAL_LIGHT, DIRECTIONAL_LIGHT_BOUNDS, CAMERA_POSITION);
	fps_counter = create_fps_counter(renderer, FPS_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN, FPS_COUNTER_SCALE, FPS_TEXT_COLOR);
}

void WorldSystem::console_debug_fps()
{
	if (debugging.show_game_fps)
	{
		std::cout << "FPS: " << fps << std::endl;
	}
}

void WorldSystem::update_fps(float elapsed_ms_since_last_update)
{
	frame_count++;
	time_accumulator += elapsed_ms_since_last_update;
	if (time_accumulator >= 1000.0f) {
		fps = frame_count * 1000.0f / time_accumulator;
		frame_count = 0;
		time_accumulator = 0.0f;

		auto& fps_component = registry.fpsCounters.get(fps_counter);
		fps_component.current_fps = fps;

		console_debug_fps();
	}
}

void WorldSystem::dead_player() {
	is_dead = true;
	registry.is_dead = true;
	auto& motions_registry = registry.motions;
	Motion& motion = motions_registry.get(player_blendy);
	motion.velocity.x = 0;
	motion.velocity.y = 0;
	motion.angle = { 0.0f };
}



// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Only interested in collisions that involve Blendy
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			// Checking Player - Minion collisions
			if (registry.minions.has(entity_other)) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity)) {
					// Kill blendy and reset death timer
					registry.deathTimers.emplace(entity);
					// add some sound effect
					// switch to dead animation
					Mix_PlayChannel(-1, dead_sound, 0);
					dead_player();
				}
			}
		}
	}
	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window)) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

void WorldSystem::move_player(vec2 direction) {
	auto& motions_registry = registry.motions;
	Motion& player_motion = motions_registry.get(player_blendy);
	float& speed = registry.players.get(player_blendy).max_speed;
	player_motion.velocity.x = direction.x * speed;
	player_motion.velocity.y = direction.y * speed;
}



void WorldSystem::update_player_movement() {
	if (is_dead) return;

	vec2 direction = { 0, 0 };
	if (keyWPressed) direction.y -= 1;
	if (keySPressed) direction.y += 1;
	if (keyAPressed) direction.x -= 1;
	if (keyDPressed) direction.x += 1;

	// Normalize direction to avoid faster diagonal movement
	if (direction.x != 0 || direction.y != 0) {
		float length = sqrt(direction.x * direction.x + direction.y * direction.y);
		direction.x /= length;
		direction.y /= length;
	}

	move_player(direction);
}


void WorldSystem::handlePlayerMovement(int key, int action) {
	bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

	switch (key) {
	case GLFW_KEY_W: keyWPressed = isPressed; break;
	case GLFW_KEY_S: keySPressed = isPressed; break;
	case GLFW_KEY_A: keyAPressed = isPressed; break;
	case GLFW_KEY_D: keyDPressed = isPressed; break;
	}
}

void WorldSystem::on_key(int key, int, int action, int mod) {
	handlePlayerMovement(key, action);

	auto& motion = registry.motions.get(directional_light);
	vec2& new_pos = motion.position;
	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_I) {
		  new_pos = { motion.position.x, motion.position.y - LIGHT_SOURCE_MOVEMENT_DISTANCE };
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_J) {
		new_pos = { motion.position.x - LIGHT_SOURCE_MOVEMENT_DISTANCE, motion.position.y };
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_K) {
		new_pos = { motion.position.x, motion.position.y + LIGHT_SOURCE_MOVEMENT_DISTANCE };
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_L) {
		new_pos = { motion.position.x + LIGHT_SOURCE_MOVEMENT_DISTANCE, motion.position.y };
	}

	// check window boundary
	if (new_pos.x < 0) new_pos.x = DIRECTIONAL_LIGHT_BB_WIDTH / 2;
	if (new_pos.y < 0) new_pos.y = DIRECTIONAL_LIGHT_BB_HEIGHT / 2;
	if (new_pos.x > window_width_px) new_pos.x = window_width_px - DIRECTIONAL_LIGHT_BB_WIDTH / 2;
	if (new_pos.y > window_height_px) new_pos.y = window_height_px - DIRECTIONAL_LIGHT_BB_HEIGHT / 2;
	motion.position = new_pos;

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// FPS Toggle
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		debugging.show_game_fps = !debugging.show_game_fps;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	float timer = 0.0f;
	Motion& motion = registry.motions.get(player_blendy);
	vec2& blendy_pos = motion.position;

	if (!is_dead) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_REPEAT) {
			if (timer == 0.0f) {
				vec2 bullet_direction = normalize(blendy_pos - mouse_position);
				createBullet(renderer, blendy_pos + bullet_direction, bullet_direction * LIGHT_SOURCE_MOVEMENT_DISTANCE);
				timer = 5.0f;
			}
			timer -= 0.1f;
			
		}
	}


}
