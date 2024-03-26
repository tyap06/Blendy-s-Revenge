// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>
#include <vector>
//vec2 normalize(const vec2&);
float duration = 0;
bool isParallel(const std::vector<vec2>&, const vec2&);
std::pair<float, float> projectOntoAxis(const std::vector<vec2>&, const vec2&);
bool projectionsOverlap(const std::pair<float, float>&, const std::pair<float, float>&);
bool checkMeshCollisionSAT(Mesh*, const Motion&, Mesh*, const Motion&, const box);
std::vector<vec2> getRectangleEdge(const Motion&, std::vector<vec2>&);
box calculate_overlap_area(const vec2&, const vec2&, const vec2&, const vec2&);
bool isPointInBox(const vec2&, const box&);
// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

bool collides(const Entity& entity1, const Entity& entity2, const Motion& motion1, const Motion& motion2)
{

	if (
		(registry.minions.has(entity1) && registry.minions.has(entity2))
		|| (registry.bullets.has(entity1) && registry.bullets.has(entity2))
		|| (registry.enemyBullets.has(entity1) && registry.minions.has(entity2))
		|| (registry.enemyBullets.has(entity2) && registry.minions.has(entity1))
		|| (registry.scoreCounters.has(entity1) || registry.scoreCounters.has(entity2))
		|| (registry.fpsCounters.has(entity1) || registry.fpsCounters.has(entity2))
		|| (registry.lightSources.has(entity1) || registry.lightSources.has(entity2))
		|| (registry.backgrounds.has(entity1) || registry.backgrounds.has(entity2))
		//|| (registry.healthBars.has(entity1) || registry.healthBars.has(entity2)) // Uncomment when healthBar container added
		|| (registry.helpScreens.has(entity1) || registry.helpScreens.has(entity2))
		)
	{
		return false;
	}

	if ((registry.mesh_collision.has(entity1) && registry.mesh_collision.has(entity2)))
	{
		// pass
	}
	else {
		return false;
	}
	const vec2 halfBB_one = get_bounding_box(motion1) / 2.f;
	const vec2 halfBB_two = get_bounding_box(motion2) / 2.f;
	vec2 center_dis = motion1.position - motion2.position;

	// check bounding box overlap first 
	if (abs(center_dis.x) < (halfBB_two.x + halfBB_one.x)
		&& abs(center_dis.y) < (halfBB_two.y + halfBB_one.y)) {
		//if (it_one != registry.motions.components.end() && it_two != registry.motions.components.end()) {
			box overlapBox = calculate_overlap_area(motion1.position, halfBB_one, motion2.position, halfBB_two);
			if (registry.meshPtrs.has(entity1) && registry.meshPtrs.has(entity2)) {
				if(registry.players.has(entity1)){
					auto& player = registry.players.get(entity1);
					if(player.up){

					} else if(player.down){
						
					} else if(player.left){
						
					} else if(player.right){

					}
				} 
				else if(registry.players.has(entity2)){
					auto& player = registry.players.get(entity2);
					if(player.up){

					} else if(player.down){
						
					} else if(player.left){
						
					} else if(player.right){

					}
				}
				else{
					Mesh* mesh_one = registry.meshPtrs.get(entity1);
					Mesh* mesh_two = registry.meshPtrs.get(entity2);
				}
				
				return checkMeshCollisionSAT(mesh_one, motion1, mesh_two, motion2, overlapBox);
			}
			/*else {
				return false;
			}*/
		//}

		//else {

			//return true;
		//}
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
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		if (registry.players.has(entity)) {
			// Vicky M1: idle animation
			// blendy animation
			Player& blendy = registry.players.get(entity);
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
			float new_y = motion.velocity.y * step_seconds + motion.position.y + motion.y_animate;
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			float half_width = bounding_box.x / 2.f;
			float half_height = bounding_box.y / 2.f;
			if (new_x - half_width > 0 && new_x + half_width < window_width_px && blendy.frame_stage != 0 && !registry.deathTimers.has(entity)) {
				motion.position.x = new_x;
			}

			if (new_y - half_height > 100 && new_y + half_height < window_height_px) {
				motion.position.y = new_y;
			}
		}

		else if(registry.minions.has(entity)){
			float new_x = motion.velocity.x * step_seconds + motion.position.x;
			float new_y = motion.velocity.y * step_seconds + motion.position.y;
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			float half_width = bounding_box.x / 2.f;
			float half_height = bounding_box.y / 2.f;
			if (new_x - half_width <= 0 || new_x + half_width >= window_width_px) {
				if (registry.roamers.has(entity)) {
					motion.velocity.x *= -1; // Invert X velocity upon boundary collision
					new_x = motion.velocity.x * step_seconds + motion.position.x; // Recalculate new_x after velocity inversion
				}
			}
			else {
				motion.position.x = new_x; // Update position if within bounds
			}

			if (new_y <= 180 || new_y + half_height-20 >= window_height_px) {
				if (registry.roamers.has(entity)) {
					motion.velocity.y *= -1; // Invert Y velocity upon boundary collision
					new_y = motion.velocity.y * step_seconds + motion.position.y; // Recalculate new_y after velocity inversion
				}
			}
			else {
				motion.position.y = new_y; // Update position if within bounds
			}
		}
		else {
			if (motion.position.x < 0.f || motion.position.x > window_width_px 
				|| motion.position.y < 0 || motion.position.y > window_height_px) {

				registry.remove_all_components_of(motion_registry.entities[i]);
				continue;
			}
			motion.position.x += motion.velocity.x * step_seconds;
			motion.position.y += motion.velocity.y * step_seconds;
		}

	}


	// Vicky TODO M1: more blood loss, the screen will trun into black, until dead
	float bloodLossPercentage;
	bloodLossPercentage = std::max(0.0f, std::min(1.0f, bloodLossPercentage)) * 100.0f;
	float alphaFactor = 1.0f - bloodLossPercentage / 100.0f;
	//render_Screen(alphaFactor);

	// Check for collisions between all moving entities
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			Entity entity_j = motion_container.entities[j];


			//if (dontCheckForCollisions(entity_i, entity_j))
			//{
			//	continue;
			//}

			if (!registry.mesh_collision.has(motion_container.entities[i]) && registry.mesh_collision.has(motion_container.entities[j])) {
				continue;
			}

			if (collides(entity_i, entity_j, motion_i, motion_j))
			{	
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);


				//if (registry.bullets.has(entity_i) && registry.minions.has(entity_j)) {
				//	/*std::cout << "minion bullet collision" << std::endl;*/
				//	if (registry.collisions.has(entity_i) && registry.collisions.get(entity_i).other == entity_j) {
				//		/*std::cout << "collision added" << std::endl;*/
				//	}

				//}
				//else if (registry.bullets.has(entity_j) && registry.minions.has(entity_i)) {
				//	/*std::cout << "minion bullet collision" << std::endl;*/
				//	if (registry.collisions.has(entity_i) && registry.collisions.get(entity_i).other == entity_j) {
				//		/*std::cout << "collision added" << std::endl;*/
				//	}
				//}
				
			}
		}
	}
}




bool checkMeshCollisionSAT(Mesh* mesh,const Motion& motion_one, Mesh* otherMesh,const Motion& motion_two, const box overlapBox) {
	std::vector<vec2> axises;
	std::vector<vec2> axises_copy;
	std::vector<vec2> edges;
	std::vector<vec2> shape;
	std::vector<vec2> otherShape;
	bool collision = false;

	Transform transform_one;
	Transform transform_two;
	transform_one.translate(motion_one.position);
	transform_one.rotate(motion_one.angle);
	transform_one.scale(motion_one.scale);
	
	transform_two.translate(motion_two.position);
	transform_two.rotate(motion_two.angle);
	transform_two.scale(motion_two.scale);

	for (size_t i = 0; i < mesh->vertex_indices.size(); i += 3) {
		axises.clear();
		edges.clear();
		shape.clear();
		vec2 positions[3];

		if (i + 2 < mesh->vertex_indices.size()) {
			for (int j = 0; j < 3; j++) {
				const ColoredVertex& v = mesh->vertices[mesh->vertex_indices[i + j]];
				// Transform the vertex position
				vec3 worldPos = transform_one.mat * vec3(v.position.x, v.position.y, 1.0f);
				positions[j] = vec2(worldPos.x, worldPos.y);
			}
			// only check polygons with indices that inside the overlap box 
			for (vec2 point: positions) {`
				if (!isPointInBox(point, overlapBox)) {
					continue;
				}
			}

			vec2 v1 = positions[0];
			vec2 v2 = positions[1];
			vec2 v3 = positions[2];
			shape.push_back(v1);
			shape.push_back(v2);
			shape.push_back(v3);
			edges.push_back(v2 - v1);
			edges.push_back(v3 - v2);
			edges.push_back(v1 - v3);

			for (const auto& edge : edges) {
				if (axises.size() == 0) {
					axises.push_back(normalize(edge));
				}
				else if (!isParallel(axises, edge)) {
					axises.push_back(normalize(edge));
				}
			}
		}
		
		// iterate the second mesh and check the polygons
		for (size_t index = 0; index < otherMesh->vertex_indices.size(); index += 3) {
			vec2 positions_2[3];
			otherShape.clear();
			edges.clear();
			axises_copy = axises;
			if (index + 2 < otherMesh->vertex_indices.size()) {
				for (int j = 0; j < 3; j++) {
					if (otherMesh->vertex_indices[index + j] < mesh->vertices.size()) {
						const ColoredVertex& v = mesh->vertices[otherMesh->vertex_indices[index + j]];
						// Transform the vertex position
						vec3 worldPos = transform_two.mat * vec3(v.position.x, v.position.y, 1.0f);
						positions_2[j] = vec2(worldPos.x, worldPos.y);
					}
					
				}
				// only check polygons with indices that inside the overlap box 
				for (vec2 point : positions_2) {
					if (!isPointInBox(point, overlapBox)) {
						continue;
					}
				}
			}

			vec2 v1 = positions_2[0];
			vec2 v2 = positions_2[1];
			vec2 v3 = positions_2[2];
			otherShape.push_back(v1);
			otherShape.push_back(v2);
			otherShape.push_back(v3);
			edges.push_back(v2 - v1);
			edges.push_back(v3 - v2);
			edges.push_back(v1 - v3);

			for (const auto& edge : edges) {
				if (axises.size() == 0) {
					axises_copy.push_back(normalize(edge));
				}
				else if (!isParallel(axises, edge)) {
					axises_copy.push_back(normalize(edge));
				}
			}

			bool haveCollision = true;
			for (const vec2 axis : axises_copy) {
				// check projection on axises
				std::pair<float, float> polygonProjection = projectOntoAxis(shape, axis);
				std::pair<float, float> rectangleProjection = projectOntoAxis(otherShape, axis);
				if (!projectionsOverlap(polygonProjection, rectangleProjection)) {
					haveCollision = false;
				}
			}

			if (haveCollision)
				return haveCollision;
		}
		
	}

	return collision;
}

//vec2 normalize(const vec2& v) {
//	float length = std::sqrt(v.x * v.x + v.y * v.y);
//	return { v.x / length, v.y / length };
//}


bool isParallel(const std::vector<vec2>& axis, const vec2& edge) {
	for (const auto& existing_axis : axis) {
		float crossProduct = edge.x * existing_axis.y - edge.y * existing_axis.x;
		if (std::abs(crossProduct) < 0.0000001) {
			return true;
		}
	}
	return false;
}

std::vector<vec2> getRectangleEdge(const Motion& motion, std::vector<vec2>& shape) {
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

	rectangle[0] = normalize(topRight - topLeft); // Top edge
	rectangle[1] = normalize(bottomRight - topRight); // Right edge
	rectangle[2] = normalize(bottomLeft - bottomRight); // Bottom edge
	rectangle[3] = normalize(topLeft - bottomLeft); // Left edge
	return rectangle;
}

//projectOntoAxis function reference https://dyn4j.org/2010/01/sat/
std::pair<float, float> projectOntoAxis(const std::vector<vec2>& shape, const vec2& axis) {
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

box calculate_overlap_area(const vec2& center1, const vec2& halfBB1, const vec2& center2, const vec2& halfBB2) {
	vec2 overlapLeftTop = { std::max(center1.x - halfBB1.x, center2.x - halfBB2.x), std::max(center1.y - halfBB1.y, center2.y - halfBB2.y) };
	vec2 overlapRightBottom = { std::min(center1.x + halfBB1.x, center2.x + halfBB2.x), std::min(center1.y + halfBB1.y, center2.y + halfBB2.y) };

	vec2 overlapCenter = (overlapLeftTop + overlapRightBottom) / 2.0f;
	vec2 overlapScale = overlapRightBottom - overlapLeftTop;
	if (overlapScale.x < 0 || overlapScale.y < 0) {
		return { {0, 0}, {0, 0} };
	}

	return { overlapCenter, overlapScale };
}

bool isPointInBox(const vec2& point, const box& bbox) {
	float left = bbox.center.x - bbox.scale.x;
	float right = bbox.center.x + bbox.scale.x;
	float bottom = bbox.center.y - bbox.scale.y;
	float top = bbox.center.y + bbox.scale.y;

	return point.x >= left && point.x <= right && point.y >= bottom && point.y <= top;
}