#include "ai_system.hpp"
#include "world_init.hpp"
#include <random>

const int update_frequency = 100;
const float ideal_range_from_player = 400.0f; 
const float approach_speed_factor = 1.0f; 
const float dodge_speed_factor = 1.5f; 
static int frame_count = 0;

std::random_device rd; 
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(-2000, 100); 

float calculateDistance(const vec2& pos1, const vec2& pos2) {
	vec2 diff = pos1 - pos2;
	return sqrt(diff.x * diff.x + diff.y * diff.y);
}

ShooterState decideShooterState(const vec2& enemyPos, const vec2& playerPos, float idealRange) {
	float distance = calculateDistance(enemyPos, playerPos);
	if (distance > idealRange) {
		return ShooterState::Approach;
	}
	else {
		return ShooterState::Dodge;
	}
}

void AISystem::shoot(Entity shooterEntity, const vec2& playerPosition, float elapsed_ms) {
	auto& shooter = registry.shooters.get(shooterEntity);
	Motion& motion = registry.motions.get(shooterEntity);

	// Update the shooting timer
	shooter.time_since_last_shot_ms += elapsed_ms;
	if (shooter.time_since_last_shot_ms >= shooter.shoot_interval_ms) {
		vec2 bullet_direction = normalize(playerPosition - motion.position);

		vec2 up_vector{ 0.0f, -1.0f };
		float bullet_angle = std::atan2(bullet_direction.y, bullet_direction.x);
		float up_angle = std::atan2(up_vector.y, up_vector.x);
		float angle_diff = bullet_angle - up_angle;
		if (angle_diff < -M_PI) {
			angle_diff += 2 * M_PI;
		}
		else if (angle_diff > M_PI) {
			angle_diff -= 2 * M_PI;
		}
		// Call the function to create and launch the bullet
		create_enemy_bullet(renderer, motion.position, bullet_direction * 300.0f, angle_diff);

		// Reset the shoot timer randomly within specified range
		shooter.time_since_last_shot_ms = static_cast<float>(distr(gen)); // Ensure distr range is properly defined elsewhere
	}
}


void AISystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
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
		float original_speed = enemy.speed;
		vec2 chase_direction = normalize(predicted_player_pos - minions_pos);

		if (enemy.type == Enemy_TYPE::BASIC) {
			motion.velocity = chase_direction * original_speed;
		}
		else if (enemy.type == Enemy_TYPE::SHOOTER) {
			ShooterState state = decideShooterState(motion.position, predicted_player_pos, ideal_range_from_player);
			switch (state) {
			case ShooterState::Approach:
				motion.velocity = chase_direction * (original_speed * approach_speed_factor);
				break;
			case ShooterState::Dodge:
				motion.velocity = -chase_direction * (original_speed * dodge_speed_factor);
				break;
			}
			shoot(enemy_enitiy, player_position, elapsed_ms * update_frequency);
		}

	}

	
}