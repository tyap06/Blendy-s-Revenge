#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;


	
  
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	bool keyWPressed = false;
	bool keySPressed = false;
	bool keyAPressed = false;
	bool keyDPressed = false;
	float bullet_timer = 0.0f;
	float elapsed_ms = 0.0f;
	float bullet_speed = 500.f;
	float bullet_launch_interval = 0.4f;
	// restart level
	void restart_game();
	void console_debug_fps();

	// Update Fps
	
	

	// OpenGL window handle
	GLFWwindow* window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	Entity player_blendy;
	Entity game_background;
	Entity directional_light;
	Entity fps_counter;
	float next_minion_spawn;
	Entity health_bar_frame;
	Entity help_screen;
	bool showHelpScreen;
	float next_dodger_spawn;
	float next_roamer_spawn;
	float next_powerup_spawn;
	Entity health_bar_box;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* dead_sound;
	Mix_Chunk* get_point;

	// fps variables
	unsigned int fps = 0;
	unsigned int frame_count = 0;
	float time_accumulator = 0.f;

	// score counter variables
	Entity score_counter;

	// Particle System
	Entity test_particle_emitter;
	Entity test_particle_emitter_2;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	void hit_player(const int& damage);
	void hit_enemy(const Entity& target, const int& damage);

	// Private Helpers For Initialization
	void update_minions(float elapsed_ms_since_last_update);
	void handlePlayerMovement(int key, int action);
	void update_player_movement();
	void move_player(vec2 direction);
	void get_blendy_render_request(bool up, bool down, bool left, bool right, int stage);
	float get_y_animate(int stage, int going_up);
	void update_fps(float elapsed_ms_since_last_update);
	void update_score();
	vec2 getCurrentMousePosition();
	void update_bullets(float elapsed_ms_since_last_update);
	void update_powerups(float elapsed_ms_since_last_update);
	void update_health_bar();
};


