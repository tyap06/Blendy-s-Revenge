// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// bool collides(const Motion& motion1, const Motion& motion2)
// {

// 	bool player1 = motion1.type == EntityType::Player;
// 	bool player2 = motion2.type == EntityType::Player;
// 	vec2 dp = motion1.position - motion2.position;
// 	float dist_squared = dot(dp, dp);
// 	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
// 	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
// 	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
// 	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
// 	const float r_squared = max(other_r_squared, my_r_squared);

// 	if (dist_squared < r_squared) {
// 		if (player1 || player2) {
// 			Mesh* mesh = registry.meshPtrs.components[0];
// 			std::vector<vec2> windowPoints = {
// 				{motion1.position.x - other_bonding_box.x, motion1.position.y - other_bonding_box.y},
// 				{motion1.position.x + other_bonding_box.x, motion1.position.y - other_bonding_box.y},
// 				{motion1.position.x - other_bonding_box.x, motion1.position.y + other_bonding_box.y},
// 				{motion1.position.x + other_bonding_box.x, motion1.position.y + other_bonding_box.y},
// 			};
// 			for (const auto& point : windowPoints) {
// 				if (pointInMesh(point, *mesh)) {
// 					return true; 
// 				}
// 			}
// 		} else {
// 			return true;
// 		}
// 	}
// 	return false;
	
// }

// //reference: https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
// bool pointInMesh(vec2 point, const Mesh& mesh) {
// 	bool inside = false;
// 	double x = point.x, y = point.y;

// 	for (size_t i = 0; i < mesh.vertex_indices.size(); i++) {
// 		vec2 p1 = { mesh.vertices[mesh.vertex_indices[i]].position.x, mesh.vertices[mesh.vertex_indices[i]].position.y };
// 		vec2 p2 = { mesh.vertices[mesh.vertex_indices[(i + 1) % mesh.vertex_indices.size()]].position.x, 
// 			mesh.vertices[mesh.vertex_indices[(i + 1) % mesh.vertex_indices.size()]].position.y };
// 		if (y > std::min(p1.y, p2.y)) {
// 			if (y <= std::max(p1.y, p2.y)) {
// 				if (x <= std::max(p1.x, p2.x)) {
// 					double x_intersection = (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
// 					if (p1.x == p2.x || x <= x_intersection) {
// 						inside = !inside;
// 					}
// 				}
// 			}
// 		}
// 	}

// 	return inside;
// }


void PhysicsSystem::step(float elapsed_ms)
{
	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		//Motion& motion = motion_registry.components[i];
		//Entity entity = motion_registry.entities[i];
		//float step_seconds = elapsed_ms / 1000.f;
		(void)elapsed_ms; // placeholder to silence unused warning until implemented
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}