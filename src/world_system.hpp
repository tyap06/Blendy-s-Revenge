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

	bool keys[GLFW_KEY_LAST] = { false };

	// 在WorldSystem类中添加一个成员函数，用于处理按键状态
	void handle_key_states(GLFWwindow* window) {
		// 示例：按下W键，更新状态数组
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			keys[GLFW_KEY_W] = true;
		}
		else {
			keys[GLFW_KEY_W] = false;
		}

		// 示例：按下A键，更新状态数组
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			keys[GLFW_KEY_A] = true;
		}
		else {
			keys[GLFW_KEY_A] = false;
		}

		// 同时按下W和A键时，可以执行相应的操作
		if (keys[GLFW_KEY_W] && keys[GLFW_KEY_A]) {
			// 执行同时按下W和A键的操作
		}
	}
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_eagle_spawn;
	float next_bug_spawn;
	Entity player_chicken;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* chicken_dead_sound;
	Mix_Chunk* chicken_eat_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
