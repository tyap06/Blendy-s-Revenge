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
		//entity,
		//{ TEXTURE_ASSET_ID::EAGLE, // change EAGLE TO BULLET
		 //EFFECT_ASSET_ID::TEXTURED,
		 //GEOMETRY_BUFFER_ID::SPRITE });
	// Vicky M1: Uncomment it later!! 
	return entity;
}

Entity createBlendy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -BLENDY_BB_WIDTH, BLENDY_BB_HEIGHT });

	// Create an (empty) Blendy component to be able to refer to Blendy
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BLENDY,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
