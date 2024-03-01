// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>
#include <vector>
vec2 normalize(const vec2&);
float duration = 0;
bool isParallel(const std::vector<vec2>&, const vec2&);
std::pair<float, float> projectOntoAxis(const std::vector<vec2>&, const vec2&);
bool projectionsOverlap(const std::pair<float, float>&, const std::pair<float, float>&);
bool checkMeshCollisionSAT(Mesh*, const Motion&);
std::vector<vec2> getRectangleEdge(const Motion&, std::vector<vec2>&);
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
	const vec2 other_halfBB = get_bounding_box(motion1) / 2.f;
	const vec2 my_halfBB = get_bounding_box(motion2) / 2.f;
	vec2 center_dis = motion1.position - motion2.position;
	//std::cout << other_halfBB.x << std::endl;
	//std::cout << other_halfBB.y << std::endl;

	if (abs(center_dis.x) < (my_halfBB.x + other_halfBB.x)
		&& abs(center_dis.y) < (my_halfBB.y + other_halfBB.y)) {

		if (motion1.type == EntityType::Player) {
			Entity& entity = registry.players.entities[0];
			Mesh* mesh = registry.meshPtrs.get(entity);
			mesh->vertices[0].position;
			//std::cout << mesh->vertices[0].position.x << std::endl;
			//std::cout << mesh->vertices[0].position.y << std::endl;

			return checkMeshCollisionSAT(mesh, motion2);
		}
		else if(motion2.type == EntityType::Player) {
			Entity& entity = registry.players.entities[0];
			Mesh* mesh = registry.meshPtrs.get(entity);
			return checkMeshCollisionSAT(mesh, motion1);
		}
		else {

			return true;
		}
	}

	return false;

}
float lerp(float start, float end, float t) {

	return start * (1 - t) + end * t;
}


void PhysicsSystem::step(float elapsed_ms)
{
	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	static float accumulatedTime = 0.0f;
	accumulatedTime += elapsed_ms;
	for(uint i = 0; i< motion_registry.size(); i++)
	{	
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;
		
		if (registry.players.has(entity)) {
			// Vicky M1: idle animation
			if (!registry.is_dead) {
				const float cycleDuration = 4000.0f;
				float cycleTime = fmod(accumulatedTime, cycleDuration) / cycleDuration;


				float normalizedTime;
				if (cycleTime < 0.5f) {
					normalizedTime = cycleTime / 0.5f;
				}
				else {
					normalizedTime = (1.0f - cycleTime) / 0.5f;
				}


				const float maxScale = 1.1f;

				motion.scale.x = lerp(BLENDY_BB_WIDTH, maxScale * BLENDY_BB_WIDTH, normalizedTime);
				motion.scale.y = lerp(BLENDY_BB_HEIGHT, maxScale * BLENDY_BB_HEIGHT, normalizedTime);
			}
			
			
			
			float new_x = motion.velocity.x * step_seconds + motion.position.x;
			float new_y = motion.velocity.y * step_seconds + motion.position.y;
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			float half_width = bounding_box.x / 2.f;
			float half_height = bounding_box.y / 2.f;
			if (new_x - half_width > 0 && new_x + half_width < window_width_px) {
				motion.position.x = new_x;
			}

			if (new_y - half_height > 0 && new_y + half_height < window_height_px) {
				motion.position.y = new_y;
			}
		}
		else {
			motion.position.x += motion.velocity.x * step_seconds;
			motion.position.y += motion.velocity.y * step_seconds;
		}
		


		
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
				if (registry.bullets.has(entity_i) && registry.minions.has(entity_j)) {
					std::cout << "minion bullet collision" << std::endl;
					if (registry.collisions.has(entity_i) && registry.collisions.get(entity_i).other == entity_j) {
						std::cout << "collision added" << std::endl;
					}

				}
				else if (registry.bullets.has(entity_j) && registry.minions.has(entity_i)) {
					std::cout << "minion bullet collision" << std::endl;
					if (registry.collisions.has(entity_i) && registry.collisions.get(entity_i).other == entity_j) {
						std::cout << "collision added" << std::endl;
					}
				}
				
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}


bool checkMeshCollisionSAT(Mesh* mesh, const Motion& motion) {
	//std::cout << "SAT check" << std::endl;

	std::vector<vec2> axises;
	std::vector<vec2> edges;
	std::vector<vec2> rectangle_shape;
	std::vector<vec2> rectangle = getRectangleEdge(motion, rectangle_shape);
	std::vector<vec2> shape;
	bool collision = false;
	for (size_t i = 0; i < mesh->vertex_indices.size(); i += 3) {
		axises.clear();
		edges.clear();
		shape.clear();
		axises = rectangle;
		if (i + 2 < mesh->vertex_indices.size()) {
			const ColoredVertex& v1 = mesh->vertices[mesh->vertex_indices[i]];
			const ColoredVertex& v2 = mesh->vertices[mesh->vertex_indices[i + 1]];
			const ColoredVertex& v3 = mesh->vertices[mesh->vertex_indices[i + 2]];
			shape.push_back(v1.position);
			shape.push_back(v2.position);
			shape.push_back(v3.position);

			edges.push_back({ v2.position.x - v1.position.x, v2.position.y - v1.position.y });
			edges.push_back({ v3.position.x - v2.position.x, v3.position.y - v2.position.y });
			edges.push_back({ v1.position.x - v3.position.x, v1.position.y - v3.position.y });
			for (const auto& edge : edges) {
				if (axises.size() == 0) {
					axises.push_back(normalize(edge));
				}
				else if (!isParallel(axises, edge)) {
					axises.push_back(normalize(edge));
				}
			} 
		}
		for (const vec2 axis: axises) {
			std::pair<float, float> polygonProjection = projectOntoAxis(shape, axis);
			std::pair<float, float> rectangleProjection = projectOntoAxis(rectangle_shape, axis);
			if (!projectionsOverlap(polygonProjection, rectangleProjection)) {
				continue;
			}
			collision = true;
		}
		
	}
	return collision;

}

vec2 normalize(const vec2& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y);
	return { v.x / length, v.y / length };
}


bool isParallel(const std::vector<vec2>& axis, const vec2& edge) {
	//std::cout << "check isParallel" << std::endl;

	for (const auto& existing_axis : axis) {
		float crossProduct = edge.x * existing_axis.y - edge.y * existing_axis.x;
		if (std::abs(crossProduct) < 0.0000001) {
			return true;
		}
	}
	return false;
}

std::vector<vec2> getRectangleEdge(const Motion& motion, std::vector<vec2>& shape) {

	//std::cout << "check getRectangleEdge" << std::endl;
	std::vector<vec2> rectangle(4);
	float halfWidth = abs(motion.scale.x) / 2.0f;
	float halfHeight = abs(motion.scale.y) / 2.0f;
	vec2 topLeft = { motion.position.x - halfWidth, motion.position.y + halfHeight };
	vec2 topRight = { motion.position.x + halfWidth, motion.position.y + halfHeight };
	vec2 bottomLeft = { motion.position.x - halfWidth, motion.position.y - halfHeight };
	vec2 bottomRight = { motion.position.x + halfWidth, motion.position.y - halfHeight };
	shape.push_back(topLeft);
	shape.push_back(topRight);
	shape.push_back(bottomLeft);
	shape.push_back(bottomRight);

	rectangle[0] = normalize({ topRight.x - topLeft.x, topRight.y - topLeft.y }); // Top edge
	rectangle[1] = normalize({ bottomRight.x - topRight.x, bottomRight.y - topRight.y }); // Right edge
	rectangle[2] = normalize({ bottomLeft.x - bottomRight.x, bottomLeft.y - bottomRight.y }); // Bottom edge
	rectangle[3] = normalize({ topLeft.x - bottomLeft.x, topLeft.y - bottomLeft.y }); // Left edge
	return rectangle;
}

//projectOntoAxis function reference https://dyn4j.org/2010/01/sat/
std::pair<float, float> projectOntoAxis(const std::vector<vec2>& shape, const vec2& axis) {
	//std::cout << "check projection" << std::endl;
	float minProj = dot(shape[0], axis);
	float maxProj = minProj;
	for (const auto& point : shape) {
		float proj = dot(point, axis);
		minProj = std::min(minProj, proj);
		maxProj = std::max(maxProj, proj);
	}
	return { minProj, maxProj };
}
bool projectionsOverlap(const std::pair<float, float>& proj1, const std::pair<float, float>& proj2) {
	return !(proj1.second < proj2.first || proj2.second < proj1.first);
}