#include "ai_system.hpp"
#include "world_init.hpp"

const int update_frequency = 50;
const float ideal_range_from_player = 450.0f; 
const float approach_speed_factor = 1.0f; // Speed factor when approaching
const float dodge_speed_factor = 1.5f; // Speed factor when dodging
static int frame_count = 0;

void AISystem::shoot(float time) {
	
}

void AISystem::step(float elapsed_ms)
{
	frame_count++;
	if (frame_count % update_frequency != 0) return;

	auto& motions_registry = registry.motions;
	auto& player_registry = registry.players;
	auto& enemy_registry = registry.minions;
	Motion& player_motion = motions_registry.get(player_registry.entities[0]);
	vec2 player_position = player_motion.position;
	vec2 player_velocity = player_motion.velocity;


	float prediction_time = update_frequency / 1000;
	vec2 predicted_player_pos = player_position + player_velocity * prediction_time;


	for (Entity enemy_enitiy : registry.minions.entities) {
		Motion& motion = motions_registry.get(enemy_enitiy);
		Minion& enemy = enemy_registry.get(enemy_enitiy);
		vec2 minions_pos = motion.position;
		vec2 direction_to_player = predicted_player_pos - minions_pos;
		float original_speed = length(motion.velocity);
		vec2 chase_direction = normalize(predicted_player_pos - minions_pos);

		if (enemy.type == Enemy_TYPE::BASIC) {
			motion.velocity = chase_direction * original_speed;
		}
		else if (enemy.type == Enemy_TYPE::SHOOTER) {
			auto& shooter = registry.shooters.get(enemy_enitiy);
			float distance_to_player = length(predicted_player_pos - minions_pos);
			if (distance_to_player > ideal_range_from_player) {
				motion.velocity = chase_direction * original_speed;
			}
			else if (distance_to_player < ideal_range_from_player) {
				motion.velocity = -chase_direction * original_speed;
			}
			shooter.time_since_last_shot_ms += elapsed_ms;
			if (shooter.time_since_last_shot_ms >= shooter.shoot_interval_ms) {
				shooter.time_since_last_shot_ms = 0.0f;
				vec2 bullet_velocity = normalize(player_position - motion.position) * 300.0f; // Example bullet speed
				create_enemy_bullet(renderer, motion.position, bullet_velocity, 0.0f); // Angle is 0 for simplicity
			}
		}


	}

	
}