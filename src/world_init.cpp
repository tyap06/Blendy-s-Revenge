#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include <random>

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}
Entity createCutScene(RenderSystem* renderer, vec2 pos, vec2 bounds, int stage)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = bounds;

	// add to component
	auto& cutScene = registry.cutScenes.emplace(entity);
	cutScene.stage = stage;
	if (stage == 1) {
		// 1.1
		cutScene.text[0] = "Meet Blendy!";
		cutScene.text[1] = "He works at";
		cutScene.text[2] = "a smoothie shop.";
		cutScene.text_position[0].x = 40;
		cutScene.text_position[0].y = window_height_px - 215;
		cutScene.text_position[1].x = 1360;
		cutScene.text_position[1].y = window_height_px - 150;
		cutScene.text_position[2].x = 1300;
		cutScene.text_position[2].y = window_height_px - 250;
		cutScene.scale = 1.3f;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_1_1,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (stage == 2) {
		// 1.2
		cutScene.text[0] = "One day,";
		cutScene.text[1] = "the manager brings";
		cutScene.text[2] = "in a replacement...";
		cutScene.scale = 1.3;
		cutScene.text_position[0].x = 230;
		cutScene.text_position[0].y = window_height_px - 80;
		cutScene.text_position[1].x = 70;
		cutScene.text_position[1].y = window_height_px - 180;
		cutScene.text_position[2].x = 80;
		cutScene.text_position[2].y = window_height_px - 280;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_1_2,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (stage == 3) {
		// 1.3
		cutScene.text[0] = "...and throws";
		cutScene.text[1] = "Blendy out.";
		cutScene.scale = 1.4;
		cutScene.text_position[0].x = 100;
		cutScene.text_position[0].y = window_height_px - 200;
		cutScene.text_position[1].x = 130;
		cutScene.text_position[1].y = window_height_px - 300;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_1_3,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (stage == 4) {
		// 1.4
		cutScene.text[0] = "Blendy vows to get revenge.";
		cutScene.scale = 1.3;
		cutScene.text_position[0].x = 800;
		cutScene.text_position[0].y = window_height_px - 180;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_1_4,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	} 
	else if (stage == 6) {
		// 2.1
		cutScene.text[0] = "Blendy's replacement";
		cutScene.text[1] = "begins to grow in power.";
		cutScene.scale = 1.5;
		cutScene.text_position[0].x = 850;
		cutScene.text_position[0].y = window_height_px - 600;
		cutScene.text_position[1].x = 850;
		cutScene.text_position[1].y = window_height_px - 700;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_2_1,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (stage == 7)
	{
		// 2.2
		// cutScene.text[0] = "He is determined to take blendy down a peg.";
		// cutScene.text_position[0].x = 100;
		// cutScene.text_position[0].y = window_height_px - 200;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CUTSCENE_2_2,
				TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (stage == 9) {
		// 3.1
		cutScene.text[0] = "Blendy is happy.";
		cutScene.scale = 1.4;
		cutScene.text_position[0].x = 1000;
		cutScene.text_position[0].y = window_height_px - 120;
		registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CUTSCENE_3_1,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE });
	}
	
	// create render request
	return entity;
}


Entity createHelpScreen(RenderSystem* renderer, vec2 pos, vec2 bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = bounds;

	registry.helpScreens.emplace(entity);
	// Create a render request for the help Screen
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_SCREEN,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createHealthBar(RenderSystem* renderer, vec2 pos, vec2 bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = bounds;

	// add health bar to panel component
	registry.panel.emplace(entity);

	// Create a render request for the health bar
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_BAR_FRAME, 
			TEXTURE_ASSET_ID::TEXTURE_COUNT, 
		EFFECT_ASSET_ID::TEXTURED, 
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createBullet(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.velocity = velocity;
	// Vicky M1: scale could change after render decided 
	motion.scale = vec2(100.0f, 100.0f);
	registry.bullets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity create_background(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;

	motion.velocity = { 0.f, 0.f };
	motion.position = position;


	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ bounds.x, bounds.y });

	// Create an (empty) background
	registry.backgrounds.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACKGROUND,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity create_blendy(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BLENDY);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;


	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create an (empty) Blendy component to be able to refer to Blendy
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BLENDY,
			TEXTURE_ASSET_ID::BLENDY_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_directional_light(RenderSystem* renderer, const vec2& position, const vec2& bounds, const vec3& camera_position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create a directional light
	auto& directional_light = registry.lightSources.emplace(entity);
	directional_light.light_color = { 1.0f,1.0f,1.0f };
	directional_light.shininess = 4.f;
	directional_light.ambientIntensity = 0.00f;
	directional_light.z_depth = 500.f;
	directional_light.camera_position = camera_position;

	renderer->setDirectionalLight(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DIRECTIONAL_LIGHT,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_fps_counter(RenderSystem* renderer, const vec2& position, const vec2& bounds, const vec3& color)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create an Fps Counter component to be able to refer to Blendy
	registry.fpsCounters.emplace(entity);
	auto& fps_text_color = registry.colors.emplace(entity);
	fps_text_color = color;

	return entity;
}

Entity create_score_counter(RenderSystem* renderer, const vec2& position, const vec2& bounds, const vec3& color)
{
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create an score counter to display the game score
	registry.scoreCounters.emplace(entity);
	auto& fps_text_color = registry.colors.emplace(entity);
	fps_text_color = color;

	return entity;
}

Entity create_minion(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);

	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, -100.f };
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create and (empty) Minion component to be able to refer to all minions
	registry.minions.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION,
			TEXTURE_ASSET_ID::MINION_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}



Entity create_powerup(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	registry.eatables.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });

	// Create and (empty) powerup component to be able to refer to all minions
	registry.powerUps.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BLENDY,
			TEXTURE_ASSET_ID::BLENDY_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}



Entity create_dodger(RenderSystem* renderer, const vec2& position, const vec2& bounds) {
	auto entity = Entity();

	
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& motion = registry.motions.emplace(entity);
	auto& minion = registry.minions.emplace(entity);
	minion.type = Enemy_TYPE::SHOOTER;
	minion.score = 25;
	motion.angle = 0.f;
	motion.velocity = { 0, -80.f };
	motion.position = position;
	motion.scale = vec2({ -bounds.x, bounds.y});
	vec3 color = { 1,0,0 };
	registry.colors.insert(entity, color);


	auto& dodger = registry.shooters.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION, 
		  TEXTURE_ASSET_ID::MINION_NM, 
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_roamer(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();


	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& motion = registry.motions.emplace(entity);
	auto& minion = registry.minions.emplace(entity);
	minion.type = Enemy_TYPE::ROAMER;
	minion.score = 25;
	motion.angle = 0.f;

	std::random_device rd; 
	std::mt19937 eng(rd()); 
	std::uniform_real_distribution<> distr(100, 150); 
	std::uniform_int_distribution<> signDistr(0, 1); 

	float velocityX = distr(eng) * (signDistr(eng) * 2 - 1); 
	float velocityY = distr(eng) * (signDistr(eng) * 2 - 1); 

	motion.velocity = { velocityX, velocityY };

	motion.position = position;
	motion.scale = vec2({ -bounds.x, bounds.y });
	vec3 color = { 0,1,0 };
	registry.colors.insert(entity, color);


	auto& dodger = registry.roamers.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION,
		  TEXTURE_ASSET_ID::MINION_NM,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity create_enemy_bullet(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.enemyBullets.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.mesh_collision.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.velocity = velocity; 
	motion.scale = vec2(100.0f, 100.0f);
	auto& bullet = registry.bullets.emplace(entity);
	bullet.friendly = false;
	vec3 color = { 0,40,0 };
	registry.colors.insert(entity, color);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}
