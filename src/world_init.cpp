#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity create_background(RenderSystem* renderer, vec2 position, vec2 bounds)
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

Entity create_blendy(RenderSystem* renderer, vec2 position, vec2 bounds)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, -10.f };
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

Entity create_directional_light(RenderSystem* renderer, vec2 position, vec2 bounds)
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
	directional_light.shininess = 9.f;
	directional_light.ambientIntensity = 0.05f;

	renderer->setDirectionalLight(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DIRECTIONAL_LIGHT,
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity create_minion(RenderSystem* renderer, vec2 position, vec2 bounds)
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