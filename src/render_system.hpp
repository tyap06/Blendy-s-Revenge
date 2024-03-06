#pragma once

#include <array>
#include <utility>
#include <iostream>
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BLENDY, mesh_path("Blendy.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MINION, mesh_path("Minion.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BULLET, mesh_path("Bullet.obj"))

		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("blendy-flat.png"),
			textures_path("blendy-flat-nm.png"),
			textures_path("minion-standing.png"),
			textures_path("minion-standing-nm.png"),
			textures_path("background.png"),
			textures_path("directional-light.png"),
			textures_path("bullet.png"),
			textures_path("fullHealthBar.png"),
			textures_path("HelpScreen.png"),
			textures_path("HealthBarFrame.png")
		/*	textures_path("blendy_frame_0.png"),
			textures_path("blendy_frame_1.png"),
			textures_path("blendy_frame_2.png"),
			textures_path("blendy_frame_3.png"),
			textures_path("blendy_frame_4.png")*/
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("egg"),
		shader_path("chicken"),
		shader_path("textured"),
		shader_path("wind"),
		shader_path("healthBar")};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();

	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	// Directional Light
	Entity& getDirectionalLight() { return directional_light; }

	void setDirectionalLight(const Entity& light) { directional_light = light; }

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

	
	// Helpers for drawTexturedMesh
	void handle_textured_rendering(Entity entity, GLuint program, const RenderRequest& render_request);
	void handle_normal_map_uniform(Entity entity, GLuint program);
	void setUsesNormalMap(bool cond, const GLuint program);
	void handle_chicken_or_egg_effect_rendering(const RenderRequest& render_request, GLuint program);
	void configure_base_uniforms(::Entity entity, const mat3& projection, Transform transform, const GLuint program, GLsizei& out_num_indices, const
	                             RenderRequest& render_request);
	void handle_health_bar_rendering(const RenderRequest& render_request, GLuint program);


	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
	Entity directional_light;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
