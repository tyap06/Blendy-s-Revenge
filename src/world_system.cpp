// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include <iostream>
// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include <fstream>



// Game configuration
const size_t MAX_MINIONS = 800;
const size_t MAX_DODGERS = 5;
const size_t MINION_DELAY_MS = 200 * 3;
const float LIGHT_SOURCE_MOVEMENT_DISTANCE = 50.0f;
const size_t MAX_POWERUPS = 25;
const size_t POWERUP_DELAY_MS = 200 * 3;

// UI
const vec3 BLENDY_COLOR = { 0.78f, 0.39f, 0.62f };
const vec3 MAGENTA = { 0.78f, 0.39f, 0.62f };

// DEFAULT START POSITIONS
const vec2 TOP_LEFT_OF_SCREEN = { 0.f,0.f };
const vec2 CENTER_OF_SCREEN = { window_width_px / 2, window_height_px / 2 };
const vec2 BOTTOM_RIGHT_OF_SCREEN = { window_width_px, window_height_px };
const vec2 BOTTOM_LEFT_OF_SCREEN = { 0, window_height_px };
const vec2 BOTTOM_RIGHT_OF_SCREEN_DIRECTIONAL_LIGHT	 = { window_width_px - DIRECTIONAL_LIGHT_BB_WIDTH / 2, window_height_px - DIRECTIONAL_LIGHT_BB_HEIGHT / 2};
const vec2 BLENDY_START_POSITION = { window_width_px / 2, window_height_px - 200 };
const vec2 HEALTH_BAR_POSITION = { 125.f, 30.f };

// BOUNDS
const vec2 BLENDY_BOUNDS = { BLENDY_BB_WIDTH, BLENDY_BB_HEIGHT };
const vec2 DIRECTIONAL_LIGHT_BOUNDS = { DIRECTIONAL_LIGHT_BB_WIDTH, DIRECTIONAL_LIGHT_BB_HEIGHT };
const vec2 BACKGROUND_BOUNDS = { BACKGROUND_BB_WIDTH, BACKGROUND_BB_HEIGHT };
const vec2 MINION_BOUNDS = { MINION_BB_WIDTH, MINION_BB_HEIGHT };
const vec2 HEALTH_BAR_BOUNDS = { 200.f, 40.f };
const vec2 HELP_SCREEN_BOUNDS = { 1250.f, 800.f };
bool is_dead = false;
const vec2 dead_velocity = { 0, 100.0f };
const float dead_angle = 3.0f;
const vec2 dead_scale = { 0, 0 };

// ANIMATION VALUES
const size_t BLENDY_FRAME_DELAY = 20 * 3;

// EYE POSITION (For Lighting Purposes)
const float CAMERA_Z_DEPTH = 1500.f;
const vec3 CAMERA_POSITION = {window_width_px / 2, window_height_px / 2, CAMERA_Z_DEPTH};

// FPS COUNTER
const vec2 FPS_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN = { 0.f, 0.f};
const vec2 FPS_COUNTER_SCALE = { 1.f,1.f };
const vec3 FPS_TEXT_COLOR = BLENDY_COLOR;

// SCORE COUNTER
const float SCORE_COUNTER_X = 30.f;
const float SCORE_COUNTER_Y = window_height_px - 160.f;
const vec2 SCORE_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN = { SCORE_COUNTER_X, SCORE_COUNTER_Y };
const vec2 SCORE_COUNTER_SCALE = { 1.f,1.f };
const vec3 SCORE_TEXT_COLOR = BLENDY_COLOR;

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
	//auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	//glfwSetCursorPosCallback(window, cursor_pos_redirect);

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

// make powerups spawn randomly on the map
void WorldSystem::update_powerups(float elapsed_ms_since_last_update)
{
	next_powerup_spawn -= elapsed_ms_since_last_update * current_speed;

	if (registry.powerUps.components.size() <= MAX_POWERUPS && next_powerup_spawn < 0.f) {
		next_powerup_spawn = (POWERUP_DELAY_MS / 2) + uniform_dist(rng) * (POWERUP_DELAY_MS / 2);

		create_powerup(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), 50.f + uniform_dist(rng) * (window_width_px - 100.f)), MINION_BOUNDS);
	}
}


void WorldSystem::update_minions(float elapsed_ms_since_last_update)
{
	elapsed_ms = elapsed_ms_since_last_update;
	next_minion_spawn -= elapsed_ms_since_last_update * current_speed;
	next_dodger_spawn -= elapsed_ms_since_last_update * current_speed;

	if (registry.minions.components.size() < MAX_MINIONS && next_minion_spawn < 0.f) {
		next_minion_spawn = MINION_DELAY_MS + uniform_dist(rng) * MINION_DELAY_MS;
		create_minion(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), 0.0f), MINION_BOUNDS);
	}
	if (registry.shooters.components.size() < MAX_DODGERS && next_dodger_spawn < 0.f) {
		next_dodger_spawn = MINION_DELAY_MS*3 + uniform_dist(rng) * (MINION_DELAY_MS);
		create_dodger(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), 0.0f), MINION_BOUNDS);
	}
}

// Update our game world
vec2 WorldSystem::getCurrentMousePosition() {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos); 
	return vec2{ (float)xpos, (float)ypos }; 
}
void WorldSystem::update_bullets(float elapsed_ms_since_last_update) {

	Motion& motion = registry.motions.get(player_blendy);
	vec2& blendy_pos = motion.position;
	vec2 mouse_position = getCurrentMousePosition();
	if (!is_dead) {
		if (bullet_timer <= 0.0f) {
			vec2 bullet_direction = normalize(mouse_position - blendy_pos);
			vec2 up_vector{ 0.0f, -1.0f };
			float bullet_angle = std::atan2(bullet_direction.y, bullet_direction.x);
			float up_angle = std::atan2(up_vector.y, up_vector.x);
			float angle_diff = bullet_angle - up_angle;
			if (angle_diff < -M_PI) {
				angle_diff += 2 * M_PI;
			}
			else if (angle_diff > M_PI) {
				angle_diff -= 2 * M_PI;
			}
			createBullet(renderer, blendy_pos, bullet_direction * bullet_speed, angle_diff);
			bullet_timer = bullet_launch_interval;
		}
		bullet_timer -= elapsed_ms_since_last_update / 1000.0f;
	}
	return;
}
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	
	update_fps(elapsed_ms_since_last_update);
	update_score();
	update_bullets(elapsed_ms_since_last_update);
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
	//update_powerups(elapsed_ms_since_last_update);

	// BLENDY ANIMATION
	Player& blendy = registry.players.get(player_blendy);
	Motion& blendy_motion = registry.motions.get(player_blendy);

	blendy.counter_ms -= elapsed_ms_since_last_update;
	if (blendy.counter_ms < 0.f) {
		blendy.counter_ms = BLENDY_FRAME_DELAY;
		if (blendy.going_up < 0) {
			blendy.frame_stage += 1;
			if (blendy.frame_stage > 4) {
				blendy.frame_stage = 4;
				blendy.going_up = 1;
			}
		}
		else {
			blendy.frame_stage -= 1;
			if (blendy.frame_stage < 0) {
				blendy.frame_stage = 0;
				blendy.going_up = -1;
			}
		}
	}
	// get what the render request status should be
	if (blendy_motion.velocity.x == 0 && blendy_motion.velocity.y == 0) {
		// just keep the current image
		registry.renderRequests.remove(player_blendy);
		registry.renderRequests.insert(
			player_blendy,
			{ TEXTURE_ASSET_ID::BLENDY,
				TEXTURE_ASSET_ID::BLENDY_NM,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		blendy.going_up = 1;
		blendy_motion.y_animate = 0.f;
	}
	else {
		// blendy is moving - calculate appropriate frame to put in render request
		registry.renderRequests.remove(player_blendy);
		get_blendy_render_request(blendy.up, blendy.down, blendy.right, blendy.left, blendy.frame_stage);
		blendy_motion.y_animate = get_y_animate(blendy.frame_stage, blendy.going_up);
	}

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
	registry.score = 0;
	game_background = create_background(renderer, CENTER_OF_SCREEN, BACKGROUND_BOUNDS);
	player_blendy = create_blendy(renderer, BLENDY_START_POSITION, BLENDY_BOUNDS);
	directional_light = create_directional_light(renderer, BOTTOM_RIGHT_OF_SCREEN_DIRECTIONAL_LIGHT, DIRECTIONAL_LIGHT_BOUNDS, CAMERA_POSITION);
	fps_counter = create_fps_counter(renderer, FPS_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN, FPS_COUNTER_SCALE, FPS_TEXT_COLOR);
	score_counter = create_score_counter(renderer, SCORE_COUNTER_TRANSLATION_FROM_BOTTOM_LEFT_OF_SCREEN, SCORE_COUNTER_SCALE, SCORE_TEXT_COLOR);
}

void WorldSystem::console_debug_fps()
{
	if (debugging.show_game_fps)
	{
		// std::cout << "FPS: " << fps << std::endl;
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

void WorldSystem::update_score()
{
	auto& score_component = registry.scoreCounters.get(score_counter);
	score_component.current_score = registry.score;
}

void WorldSystem::hit_player(int damage) {
	if (!registry.deathTimers.has(player_blendy)) {
		auto& player = registry.players.get(player_blendy);
		if (player.health - damage <= 0) {
			is_dead = true;
			registry.is_dead = true;
			auto& motions_registry = registry.motions;
			Motion& motion = motions_registry.get(player_blendy);
			motion.velocity.x = 0;
			motion.velocity.y = 0;
			motion.angle = { 0.0f };
			registry.deathTimers.emplace(player_blendy);
			Mix_PlayChannel(-1, dead_sound, 0);
		}
		else {
			player.health -= damage;
		}
	}
}

void WorldSystem::hit_enemy(Entity& target, int damage) {
	Minion& minion = registry.minions.get(target);
	minion.health -= damage;
	if (minion.health <= 0) {
		registry.score += minion.score;

		std::cout << registry.score << std::endl;
		registry.remove_all_components_of(target);
	}
}



// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.players.has(entity)) {
			if (registry.minions.has(entity_other)) {
				int damage = registry.minions.get(entity_other).damage;
				hit_player(damage);
				registry.remove_all_components_of(entity_other);
			}
			else if (registry.bullets.has(entity_other)) {
				if (!registry.bullets.get(entity_other).friendly) {
					int damage = registry.bullets.get(entity_other).damage;
					registry.remove_all_components_of(entity_other);
					hit_player(damage);
				}
			}
		}
		else if (registry.bullets.has(entity)) {
			auto& bullet = registry.bullets.get(entity);
			if (registry.minions.has(entity_other)&&bullet.friendly) {
				int damage = registry.bullets.get(entity).damage;
				hit_enemy(entity_other, damage);
				registry.remove_all_components_of(entity);
			}
		}
	}
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

	Player& blendy = registry.players.get(player_blendy);

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

	// BLENDY ANIMATION
	blendy.up = false;
	blendy.down = false;
	blendy.left = false;
	blendy.right = false;
	if (direction.y == 0  && direction.x > 0) {
		// going right
		blendy.right = true;
	} 
	else if (direction.y == 0  && direction.x < 0) {
		// going left
		blendy.left = true;
	}
	else if (direction.y > 0  && direction.x == 0) {
		// going down
		blendy.down = true;
	}
	else if (direction.y < 0  && direction.x == 0) {
		// going up
		blendy.up = true;
	}
	else {
		// other direction - setting blendy as down for now bc I don't have the diagonal images done
		blendy.down = true;
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

	// Toggle the help screen visibility when "H" is pressed
	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		if (!showHelpScreen) {
			// Create the help screen if it's not already shown
			help_screen = createHelpScreen(renderer, CENTER_OF_SCREEN, HELP_SCREEN_BOUNDS);
		}
		else {
			// Destroy the help screen if it's already shown
			registry.remove_all_components_of(help_screen);
		}

		// Toggle the showHelpScreen flag
		showHelpScreen = !showHelpScreen;
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

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_MINUS) {
			float currentMusicVolume = Mix_VolumeMusic(-1);
			float dead_sound_volume = Mix_VolumeChunk(dead_sound, -1);
			float get_point_volume = Mix_VolumeChunk(get_point, -1);
			Mix_VolumeMusic(currentMusicVolume - 10);
			Mix_VolumeChunk(dead_sound, dead_sound_volume - 10);
			Mix_VolumeChunk(get_point, get_point_volume - 10);
		}
		else if (key == GLFW_KEY_EQUAL) {
			float currentMusicVolume = Mix_VolumeMusic(-1);
			float dead_sound_volume = Mix_VolumeChunk(dead_sound, -1);
			float get_point_volume = Mix_VolumeChunk(get_point, -1);
			Mix_VolumeMusic(currentMusicVolume + 10);
			Mix_VolumeChunk(dead_sound, dead_sound_volume + 10);
			Mix_VolumeChunk(get_point, get_point_volume + 10);
		}
	}
}

float WorldSystem::get_y_animate(int stage, int going_up) {
	if (registry.deathTimers.has(player_blendy)) {
		return 0;
	}
	if (stage == 0) {
		return 0.f * going_up;
	}
	else if (stage == 1) {
		return 1.f * going_up;
	}
	else if (stage == 2) {
		return 2.f * going_up;
	}
	else if (stage == 3) {
		return 6.f * going_up;
	}
	else if (stage == 4) {
		return 7.f * going_up;
	}
	else {
		return 0.f * going_up;
	}
}

void WorldSystem::get_blendy_render_request(bool up, bool down, bool right, bool left, int stage) {
	// BLENDY ANIMATION
	if (up) {
		// going up
		if (stage == 0) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::UFRAME_0,
					TEXTURE_ASSET_ID::UFRAME_0_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 1) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::UFRAME_1,
					TEXTURE_ASSET_ID::UFRAME_1_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 2) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::UFRAME_2,
					TEXTURE_ASSET_ID::UFRAME_2_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 3) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::UFRAME_3,
					TEXTURE_ASSET_ID::UFRAME_3_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 4) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::UFRAME_4,
					TEXTURE_ASSET_ID::UFRAME_4_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else if (down) {
		// going down
		if (stage == 0) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::DFRAME_0,
					TEXTURE_ASSET_ID::DFRAME_0_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 1) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::DFRAME_1,
					TEXTURE_ASSET_ID::DFRAME_1_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 2) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::DFRAME_2,
					TEXTURE_ASSET_ID::DFRAME_2_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 3) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::DFRAME_3,
					TEXTURE_ASSET_ID::DFRAME_3_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 4) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::DFRAME_4,
					TEXTURE_ASSET_ID::DFRAME_4_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else if (right) {
		// going right
		if (stage == 0) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::RFRAME_0,
					TEXTURE_ASSET_ID::RFRAME_0_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 1) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::RFRAME_1,
					TEXTURE_ASSET_ID::RFRAME_1_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 2) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::RFRAME_2,
					TEXTURE_ASSET_ID::RFRAME_2_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 3) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::RFRAME_3,
					TEXTURE_ASSET_ID::RFRAME_3_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 4) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::RFRAME_4,
					TEXTURE_ASSET_ID::RFRAME_4_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else if (left) {
		// going left
		if (stage == 0) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::LFRAME_0,
					TEXTURE_ASSET_ID::LFRAME_0_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 1) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::LFRAME_1,
					TEXTURE_ASSET_ID::LFRAME_1_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 2) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::LFRAME_2,
					TEXTURE_ASSET_ID::LFRAME_2_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 3) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::LFRAME_3,
					TEXTURE_ASSET_ID::LFRAME_3_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (stage == 4) {
			registry.renderRequests.insert(
				player_blendy,
				{ TEXTURE_ASSET_ID::LFRAME_4,
					TEXTURE_ASSET_ID::LFRAME_4_NM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
}




