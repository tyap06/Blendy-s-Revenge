// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::handle_normal_map_uniform(Entity entity, const GLuint program)
{
	GLuint normal_map_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_normal_map];

	// Enabling and binding normal map texture to slot 1
	glActiveTexture(GL_TEXTURE1);
	gl_has_errors();
	glBindTexture(GL_TEXTURE_2D, normal_map_id);
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint normal_map_uloc = glGetUniformLocation(program, "normal_map");
	assert(normal_map_uloc >= 0);
	glUniform1i(normal_map_uloc, 1);
	gl_has_errors();
}

void RenderSystem::setUsesNormalMap(bool cond, const GLuint program)
{
	// Getting uniform locations for glUniform* calls
	GLint usesNormalMap_uloc = glGetUniformLocation(program, "usesNormalMap");
	glUniform1i(usesNormalMap_uloc, cond);
	gl_has_errors();
}

void RenderSystem::handle_textured_rendering(Entity entity, const GLuint program, const RenderRequest& render_request)
{
	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(TexturedVertex), (void *)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(
		in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		(void *)sizeof(
			vec3)); // note the stride to skip the preceeding vertex position
	gl_has_errors();

	assert(registry.renderRequests.has(entity));
	GLuint base_texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

	// Enabling and binding base texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	gl_has_errors();
	glBindTexture(GL_TEXTURE_2D, base_texture_id);
	gl_has_errors();

	// Handle normal map uniform if normal map exists for texture
	if (render_request.used_normal_map != TEXTURE_ASSET_ID::TEXTURE_COUNT)
	{
		handle_normal_map_uniform(entity, program);
	}
}

void RenderSystem::handle_chicken_or_egg_effect_rendering(const RenderRequest& render_request, const GLuint program)
{
	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_color_loc = glGetAttribLocation(program, "in_color");
	gl_has_errors();

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(ColoredVertex), (void *)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(ColoredVertex), (void *)sizeof(vec3));
	gl_has_errors();

	if (render_request.used_effect == EFFECT_ASSET_ID::CHICKEN)
	{
		// Light up?
		GLint light_up_uloc = glGetUniformLocation(program, "light_up");
		assert(light_up_uloc >= 0);

		// !!! TODO A1: set the light_up shader variable using glUniform1i,
		// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
		gl_has_errors();
	}
}

void RenderSystem::configure_base_uniforms(Entity entity, const mat3& projection, Transform transform, const GLuint program, GLsizei& out_num_indices, const RenderRequest& render_request)
{

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	setUsesNormalMap(render_request.used_normal_map != TEXTURE_ASSET_ID::TEXTURE_COUNT, program);

	// Lighting Config
	const LightSource& directional_light_component = registry.lightSources.get(directional_light);
	const Motion& motion = registry.motions.get(directional_light);

	// Configuring lightPosition 
	GLint lightPosition_uloc = glGetUniformLocation(program, "lightPosition");
	glUniform3fv(lightPosition_uloc, 1, (float*)& vec3(motion.position, directional_light_component.z_depth));
	gl_has_errors();

	// Configuring lightColor
	GLint lightColor_uloc = glGetUniformLocation(program, "lightColor");
	glUniform3fv(lightColor_uloc, 1, (float*)&directional_light_component.light_color);
	gl_has_errors();

	// Configuring shinyness
	GLint shininess_uloc = glGetUniformLocation(program, "shininess");
	glUniform1f(shininess_uloc, (float) directional_light_component.shininess);
	gl_has_errors();

	// Configuring ambientIntensity
	GLint ambientIntensity_uloc = glGetUniformLocation(program, "ambientIntensity");
	glUniform1f(ambientIntensity_uloc, (float)directional_light_component.ambientIntensity);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	out_num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = out_num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
}

// TODO: A number of code smells in this function that need to be cleaned up
void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		handle_textured_rendering(entity, program, render_request);
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::CHICKEN || render_request.used_effect == EFFECT_ASSET_ID::EGG)
	{
		handle_chicken_or_egg_effect_rendering(render_request, program);
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	GLsizei out_num_indices;
	configure_base_uniforms(entity, projection, transform, program, out_num_indices, render_request);

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, out_num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WIND]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint wind_program = effects[(GLuint)EFFECT_ASSET_ID::WIND];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(wind_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(wind_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(wind_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.66, 0.73, 0.76 , 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}