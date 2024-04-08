#pragma once

#include <array>
#include <map>
#include <utility>
#include <iostream>
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// Fonts
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

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
			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BLENDY, mesh_path("Blendy-Reduced.obj")),
			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MINION, mesh_path("Minion-Reduced.obj")),
			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BULLET, mesh_path("Bullet-color.obj")),
			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MINION_BULLET, mesh_path("Minion_Bullet-color.obj")),
		  
			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::GRAPE, mesh_path("grape.obj")),

			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::LEMON, mesh_path("lemon.obj")),

			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ORANGE, mesh_path("orange.obj")),

			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PROTEIN_POWER, mesh_path("protein_powder.obj")),

			std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BATTERY, mesh_path("battery.obj")),


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
			textures_path("blendy_left_0.png"),
			textures_path("blendy_left_1.png"),
			textures_path("blendy_left_2.png"),
			textures_path("blendy_left_3.png"),
			textures_path("blendy_right_0.png"),
			textures_path("blendy_right_1.png"),
			textures_path("blendy_right_2.png"),
			textures_path("blendy_right_3.png"),
			textures_path("blendy_down_0.png"),
			textures_path("blendy_down_1.png"),
			textures_path("blendy_down_2.png"),
			textures_path("blendy_down_3.png"),
			textures_path("blendy_up_0.png"),
			textures_path("blendy_up_1.png"),
			textures_path("blendy_up_2.png"),
			textures_path("blendy_up_3.png"),
			textures_path("blendy_left_0_nm.png"),
			textures_path("blendy_left_1_nm.png"),
			textures_path("blendy_left_2_nm.png"),
			textures_path("blendy_left_3_nm.png"),
			textures_path("blendy_right_0_nm.png"),
			textures_path("blendy_right_1_nm.png"),
			textures_path("blendy_right_2_nm.png"),
			textures_path("blendy_right_3_nm.png"),
			textures_path("blendy_down_0_nm.png"),
			textures_path("blendy_down_1_nm.png"),
			textures_path("blendy_down_2_nm.png"),
			textures_path("blendy_down_3_nm.png"),
			textures_path("blendy_up_0_nm.png"),
			textures_path("blendy_up_1_nm.png"),
			textures_path("blendy_up_2_nm.png"),
			textures_path("blendy_up_3_nm.png"),
			textures_path("bullet.png"),
			textures_path("bullet_nm.png"),
			textures_path("fullHealthBar.png"),
			textures_path("HelpScreen.png"),
			textures_path("HealthBarFrame.png"),
			textures_path("cutscene_1_1.png"),
			textures_path("cutscene_1_2.png"),
			textures_path("cutscene_1_3.png"),
			textures_path("cutscene_1_4.png"),
			textures_path("cutscene_2_1.png"),
			textures_path("cutscene_2_2.png"),
			textures_path("cutscene_3_1.png"),
			textures_path("title_screen.png"),
			textures_path("m_left_0.png"),
			textures_path("m_left_1.png"),
			textures_path("m_left_2.png"),
			textures_path("m_right_0.png"),
			textures_path("m_right_1.png"),
			textures_path("m_right_2.png"),
			textures_path("m_up_0.png"),
			textures_path("m_up_1.png"),
			textures_path("m_up_2.png"),
			textures_path("m_down_0.png"),
			textures_path("m_down_1.png"),
			textures_path("m_down_2.png"),
			textures_path("m_left_0_n.png"),
			textures_path("m_left_1_n.png"),
			textures_path("m_left_2_n.png"),
			textures_path("m_right_0_n.png"),
			textures_path("m_right_1_n.png"),
			textures_path("m_right_2_n.png"),
			textures_path("m_up_0_n.png"),
			textures_path("m_up_1_n.png"),
			textures_path("m_up_2_n.png"),
			textures_path("m_down_0_n.png"),
			textures_path("m_down_1_n.png"),
			textures_path("m_down_2_n.png"),

			textures_path("battery.png"),
			textures_path("orange.png"),
			textures_path("lemon.png"),
			textures_path("grape.png"),
			textures_path("protein powder.png"),
			textures_path("battery_nm.png"),
			textures_path("orange_nm.png"),
			textures_path("lemon_nm.png"),
			textures_path("grape_nm.png"),
			textures_path("protein_powder_nm.png"),
			textures_path("bright_bot_0.png"),
			textures_path("bright_bot_1.png"),
			textures_path("bright_bot_2.png"),
			textures_path("bright_bot_3.png"),
			textures_path("bright_top_0.png"),
			textures_path("bright_top_1.png"),
			textures_path("bright_top_2.png"),
			textures_path("bright_top_3.png"),
			textures_path("bleft_bot_0.png"),
			textures_path("bleft_bot_1.png"),
			textures_path("bleft_bot_2.png"),
			textures_path("bleft_bot_3.png"),
			textures_path("bleft_top_0.png"),
			textures_path("bleft_top_1.png"),
			textures_path("bleft_top_2.png"),
			textures_path("bleft_top_3.png"),

			textures_path("bright_bot_0_n.png"),
			textures_path("bright_bot_1_n.png"),
			textures_path("bright_bot_2_n.png"),
			textures_path("bright_bot_3_n.png"),
			textures_path("bright_top_0_n.png"),
			textures_path("bright_top_1_n.png"),
			textures_path("bright_top_2_n.png"),
			textures_path("bright_top_3_n.png"),
			textures_path("bleft_bot_0_n.png"),
			textures_path("bleft_bot_1_n.png"),
			textures_path("bleft_bot_2_n.png"),
			textures_path("bleft_bot_3_n.png"),
			textures_path("bleft_top_0_n.png"),
			textures_path("bleft_top_1_n.png"),
			textures_path("bleft_top_2_n.png"),
			textures_path("bleft_top_3_n.png"),
			textures_path("cherry.png"),
			textures_path("nut.png"),
			textures_path("cactus.png")

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

	// Fonts
	std::map<char, Character> m_ftCharacters;
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;

	const std::string FONT_FOLDER_PATH = "..//..//..//data//fonts//";
	const std::string DOT_TTF = ".ttf";
	const std::string Kenney_Pixel_Square = "Kenney_Pixel_Square";
	const std::string Kenney_Future_Narrow = "Kenney_Future_Narrow";
	const unsigned int FONT_DEFAULT_SIZE = 48;


	// Dummy VAO
	GLuint dummy_vao;

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

	// Fonts
	bool initializeFonts();
	bool fontInit_internal(const std::string& font_filename, unsigned font_default_size);
	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans);


private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();


	// Helpers for drawTexturedMesh
	void handle_textured_rendering(Entity entity, GLuint program, const RenderRequest& render_request);
	void handle_normal_map_uniform(Entity entity, GLuint program);
	void setUsesNormalMap(bool cond, const GLuint program);
	void handle_chicken_or_egg_effect_rendering(const RenderRequest& render_request, GLuint program);
	void configure_base_uniforms(::Entity entity, const mat3& projection, Transform transform, const GLuint program, GLsizei& out_num_indices, const RenderRequest& render_request);
	void handle_giant_uniform(const Entity entity, const GLuint program);
	                             
	void handle_health_bar_rendering(const RenderRequest& render_request, GLuint program);


	// Debugging FPS
	void debug_fps(const mat3& projection);

	// Score Counter
	void display_score();

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
