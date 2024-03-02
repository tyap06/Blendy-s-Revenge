#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class EntityType {
	Generic,
	Player,
};

enum class POWERUP_TYPE {

	PAC_FRUIT = 0,
	LASER = PAC_FRUIT + 1,
	PROTIEN = LASER + 1,
};

// Player component
struct Player
{
	float max_speed = 200.f;
	int health = 5;
	int max_effect = 3;
	int current_effect = 0;
	bool pac_mode = false;
	//int frame_stage = 1;
	//bool left = false;
	//bool right = false;
	//bool up = false;
	//bool down = true;
};



struct PowerUp
{
	POWERUP_TYPE type;
	float duration_ms = 0.0f;
	int count = 0;
};

struct Minion
{

};

struct Eatable
{
	int power_up_id = -1;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	EntityType type = EntityType::Generic;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };

};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 3000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// Background component for if an entity represents a background image
struct Background
{
	
};

// LightSource component for entities that represent a LightSource
struct LightSource
{
	vec3 light_color;

	// Parameter for Blinn-Phong
	float shininess;

	// Intensity of ambient light to add to scene b/w [0.0, 1.0]
	float ambientIntensity;

	// Z-depth of the light
	float z_depth;

	// Position of camera for doing lighting calculations
	vec3 camera_position;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	BLENDY = 0,
	BLENDY_NM = BLENDY + 1,
	MINION = BLENDY_NM + 1,
	MINION_NM = MINION + 1,
	BACKGROUND = MINION_NM + 1,
	DIRECTIONAL_LIGHT = BACKGROUND + 1,
	//LFRAME_0 = DIRECTIONAL_LIGHT + 1,
	//LFRAME_1 = LFRAME_0 + 1,
	//LFRAME_2 = LFRAME_1 + 1,
	//LFRAME_3 = LFRAME_2 + 1,
	//LFRAME_4 = LFRAME_3 + 1,
	//TEXTURE_COUNT = LFRAME_4 + 1
	TEXTURE_COUNT = DIRECTIONAL_LIGHT + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	EGG = COLOURED + 1,
	CHICKEN = EGG + 1,
	TEXTURED = CHICKEN + 1,
	WIND = TEXTURED + 1,
	EFFECT_COUNT = WIND + 1
};

const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	CHICKEN = 0,
	SPRITE = CHICKEN + 1,
	EGG = SPRITE + 1,
	DEBUG_LINE = EGG + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	TEXTURE_ASSET_ID used_normal_map = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

