#include "ai_system.hpp"

const float safe_bug_distance = 300.0f;
const int update_frequency = 100;


void AISystem::step(float elapsed_ms)
{
	static int frame_count = 0;
	frame_count++;

	auto& motions_registry = registry.motions;
	auto& player_registry = registry.players;
	Motion& player_motion = motions_registry.get(player_registry.entities[0]);
	vec2 player_position = player_motion.position;
	vec2 player_velocity = player_motion.velocity;


	float prediction_time = update_frequency / 1000;
	vec2 predicted_player_pos = player_position + player_velocity * prediction_time;

	for (int i = 0; i < motions_registry.components.size(); ++i) {
		Motion& motion = motions_registry.components[i];


		if (registry.minions.has(motions_registry.entities[i])) {
			vec2 minions_pos = motion.position;
			vec2 direction_to_player = direction_to_player - minions_pos;
			float original_speed = length(motion.velocity);


			if (frame_count % update_frequency != 0) return;
			vec2 flee_direction = normalize(minions_pos - predicted_player_pos);
			motion.velocity = -flee_direction * original_speed;
		}

	}
}