#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>

Entity createBullet(RenderSystem* renderer, vec2 pos, vec2 velocity) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object, like createChicken
	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	// registry.meshPtrs.emplace(entity, &mesh);
	std::cout << "Left button pressed" << std::endl;  // Debug message
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = velocity;
	// Vicky M1: scale could change after render decided 
	motion.scale = vec2(1.0f, 1.0f);

	// Create and (empty) Eagle component to be able to refer to all eagles
	// TODO
	//registry.deadlys.emplace(entity);
	//registry.renderRequests.insert(
	//	entity,
	//	{ TEXTURE_ASSET_ID::EAGLE, // change EAGLE TO BULLET
	//	 EFFECT_ASSET_ID::TEXTURED,
	//	 GEOMETRY_BUFFER_ID::SPRITE });
	// Vicky M1: Uncomment it later!! 
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
	//motion.type = EntityType::Generic;


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
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	//motion.type = EntityType::Generic;

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

Entity create_minion(RenderSystem* renderer, const vec2& position, const vec2& bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 100.f };
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

