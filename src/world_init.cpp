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
	motion.mesh_scale = motion.scale;

	registry.debugComponents.emplace(entity);
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
	motion.mesh_scale = motion.scale;

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
	motion.mesh_scale = motion.scale;

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
	motion.scale = vec2(40.f, 100.f);
	motion.mesh_scale = vec2(10.f, 1000.f);
	registry.bullets.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::CHICKEN,
		 GEOMETRY_BUFFER_ID::BULLET});

/*	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE })*/;
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
	motion.mesh_scale = motion.scale;

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
	motion.mesh_scale = motion.scale;
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
	motion.mesh_scale = motion.scale;

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
	motion.mesh_scale = motion.scale;

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
	motion.mesh_scale = motion.scale;

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
	motion.mesh_scale = motion.scale;
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

Entity create_giant(RenderSystem* renderer, const vec2& position, const vec2& bounds, int score)
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
	motion.scale = vec2({ -bounds.x, bounds.y });
	auto& minion = registry.minions.emplace(entity);
	minion.score = 200;
	minion.health = score/2;
	minion.max_health = score/2;
	minion.speed = 50;
	
	registry.giants.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION,
			TEXTURE_ASSET_ID::MINION_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}




Entity create_battery_powerup(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 25.f;
	motion.velocity = { 0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });
	motion.mesh_scale = motion.scale;

	// Create and (empty) powerup component to be able to refer to all minions
	auto& powerup = registry.powerUps.emplace(entity);
	powerup.type = POWERUP_TYPE::BATTERY;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BATTERY_POWERUP,
			TEXTURE_ASSET_ID::BATTERY_POWERUP_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_protein_powerup(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 25.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });
	motion.mesh_scale = motion.scale;

	// Create and (empty) powerup component to be able to refer to all minions
	auto& powerup = registry.powerUps.emplace(entity);
	powerup.type = POWERUP_TYPE::PROTEIN;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PROTEIN_POWERUP,
			TEXTURE_ASSET_ID::PROTEIN_POWERUP_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_grape_powerup(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });
	motion.mesh_scale = motion.scale;

	// Create and (empty) powerup component to be able to refer to all minions
	auto& powerup = registry.powerUps.emplace(entity);
	powerup.type = POWERUP_TYPE::GRAPE;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GRAPE_POWERUP,
			TEXTURE_ASSET_ID::GRAPE_POWERUP_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_lemon_powerup(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.mesh_collision.emplace(entity);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 25.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -bounds.x, bounds.y });
	motion.mesh_scale = motion.scale;
	// Create and (empty) powerup component to be able to refer to all minions
	auto& powerup = registry.powerUps.emplace(entity);
	powerup.type = POWERUP_TYPE::LEMON;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEMON_POWERUP,
			TEXTURE_ASSET_ID::LEMON_POWERUP_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_lemon_bullet(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.mesh_collision.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.velocity = velocity;
	motion.scale = vec2(100.0f, 100.0f);
	motion.mesh_scale = vec2(10.f, 1000.f);
	auto& bullet = registry.bullets.emplace(entity);

	vec3 color = { 40,40,0 };
	registry.colors.insert(entity, color);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}



Entity create_fast_bullet(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.mesh_collision.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.velocity = velocity;
	motion.scale = vec2(100.0f, 100.0f);
	motion.mesh_scale = vec2(10.f, 1000.f);
	auto& bullet = registry.bullets.emplace(entity);
	
	vec3 color = { 40,0,0 };
	registry.colors.insert(entity, color);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
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
	motion.mesh_scale = motion.scale;
	vec3 color = { 0.8,0.8,0.0 };
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

Entity create_charger(RenderSystem* renderer, const vec2& position, const vec2& bounds) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& motion = registry.motions.emplace(entity);
	auto& minion = registry.minions.emplace(entity);
	minion.type = Enemy_TYPE::CHARGER;
	minion.score = 40;
	motion.angle = 0.f;
	minion.health = 40;
	minion.max_health = 40;
	minion.armor = 15;
	motion.velocity = { 0, -80.f };
	motion.position = position;
	motion.scale = vec2({ -bounds.x, bounds.y });
	vec3 color = {0.5,0.2,0.2 };
	registry.colors.insert(entity, color);

	auto& charger = registry.chargers.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION,
		  TEXTURE_ASSET_ID::MINION_NM,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_tank(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& motion = registry.motions.emplace(entity);
	auto& minion = registry.minions.emplace(entity);
	minion.type = Enemy_TYPE::TANK;
	minion.score = 80;
	minion.armor = 20;
	motion.angle = 0.f;
	minion.health = 100;
	minion.max_health = minion.health;
	minion.speed = 50;
	motion.velocity = { 0, -80.f };
	motion.position = position;
	motion.scale = vec2({ -bounds.x, bounds.y });
	vec3 color = { 0,0,0 };
	registry.colors.insert(entity, color);

	auto& tank = registry.tanks.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MINION,
		  TEXTURE_ASSET_ID::MINION_NM,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_sniper(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION);
	registry.mesh_collision.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& motion = registry.motions.emplace(entity);
	auto& minion = registry.minions.emplace(entity);
	minion.type = Enemy_TYPE::SNIPER;
	minion.score = 50;
	motion.angle = 0.f;
	motion.velocity = { 0, -80.f };
	minion.health = 50;
	minion.max_health = 50;
	motion.position = position;
	motion.scale = vec2({ -bounds.x, bounds.y });
	vec3 color = { 0.2,0.8,0.8 };
	registry.colors.insert(entity, color);


	auto& dodger = registry.snipers.emplace(entity);

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
	minion.score = 30;
	minion.speed = 100;
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
	motion.mesh_scale = motion.scale;
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




Entity create_enemy_bullet(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, int damage, vec3 color) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::MINION_BULLET);
	registry.enemyBullets.emplace(entity);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.mesh_collision.emplace(entity);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.velocity = velocity; 
	motion.scale = vec2(40.0f, 100.0f);
	motion.mesh_scale = vec2(50.0f, 100.0f);
	auto& bullet = registry.bullets.emplace(entity);
	bullet.friendly = false;
	bullet.damage = damage;
	registry.colors.insert(entity, color);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			TEXTURE_ASSET_ID::BULLET_NM,
		 EFFECT_ASSET_ID::CHICKEN,
		GEOMETRY_BUFFER_ID::MINION_BULLET });
	return entity;
}
