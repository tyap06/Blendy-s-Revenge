#include "ai_system.hpp"
#include "world_init.hpp"
#include <random>

const int update_frequency = 50;
const float ideal_range_from_player = 400.0f; 
const float approach_speed_factor = 1.0f; 
const float dodge_speed_factor = 1.5f; 
static int frame_count = 0;
const float charger_aggro_range = 450.0f;
const float charger_aim_time = 50.0f;
const float charger_rest_time = 80.0f;
const float charger_charge_speed = 4.0f;

std::random_device rd; 
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(-2000, 100); 

float calculateDistance(const vec2& pos1, const vec2& pos2) {
	vec2 diff = pos1 - pos2;
	return sqrt(diff.x * diff.x + diff.y * diff.y);
}

vec2 calculateInterceptPosition(vec2 sniperPos, vec2 playerPos, float factor) {
	// Calculates a position factor% of the way from playerPos to sniperPos
	return playerPos + (sniperPos - playerPos) * factor;
}

Entity AISystem::findClosestSniper(vec2 tank_pos) {
	float minDistance = std::numeric_limits<float>::max();
	Entity closestSniper = registry.players.entities[0]; 

	for (auto sniperEntity : registry.snipers.entities) {
		if (registry.protections.has(sniperEntity)) {
			continue;
		}
		auto& sniperMotion = registry.motions.get(sniperEntity);
		float distance = calculateDistance(tank_pos, sniperMotion.position);

		if (distance < minDistance) {
			minDistance = distance;
			closestSniper = sniperEntity;
		}
	}

	return closestSniper;
}

void AISystem::updateTank(Entity tankEntity, vec2 chase_direction,
	Minion& enemy, Motion& motion, float elapsed_ms, vec2 player_pos) {
	auto& tank = registry.tanks.get(tankEntity);
	float distanceToPlayer = calculateDistance(motion.position, player_pos);

	switch (tank.state) {
	case Tank_state::defualt: {
		if (registry.snipers.entities.size() != 0) {
			Entity closestSniper = findClosestSniper(motion.position);
			if (!registry.players.has(closestSniper)) {
				auto& sniper_protect = registry.protections.emplace(closestSniper);
				sniper_protect.link = tankEntity;
				auto& tank_protect = registry.protections.emplace(tankEntity);
				tank_protect.link = closestSniper;
				tank.state = Tank_state::protecting;
			}	
		}
		motion.velocity = chase_direction * enemy.speed;
		break;
	}
	case Tank_state::protecting: {
		auto& protect = registry.protections.get(tankEntity);
		if (!registry.protections.has(protect.link)) {
			registry.protections.remove(tankEntity);
			tank.state = Tank_state::defualt;
			motion.velocity = chase_direction * enemy.speed;
			break;
		}
		else {
			auto& sniperMotion = registry.motions.get(protect.link);
			vec2 protectivePos = calculateInterceptPosition(sniperMotion.position, player_pos, 0.7f);
			vec2 directionToProtectivePos = normalize(protectivePos - motion.position);
			motion.velocity = directionToProtectivePos * enemy.speed;
		}
		break;
	}
	}

}


void AISystem::updateSniper(Entity sniperEntity, vec2 chase_direction,
	Minion& enemy, Motion& motion, float elapsed_ms, vec2 player_pos) {
	auto& sniper = registry.snipers.get(sniperEntity); 

	float distanceToPlayer = calculateDistance(motion.position, player_pos);
	float aimDistance = 700.0f; 
	float avoidDistance = 500.0f; 


	if (registry.protections.has(sniperEntity)) {
		auto& protect = registry.protections.get(sniperEntity);
		if (!registry.protections.has(protect.link)) {
			registry.protections.remove(sniperEntity);
		}
	}

	switch (sniper.state) {
	case Sniper_State::Avoiding:
		if (distanceToPlayer < avoidDistance) {
			vec2 flee_direction = normalize(motion.position - player_pos);
			motion.velocity = -chase_direction * enemy.speed;
		}
		else if (distanceToPlayer >= avoidDistance && distanceToPlayer <= aimDistance) {
			sniper.state = Sniper_State::Aiming;
			sniper.aim_timer = charger_aim_time; 
		}
		else {
			motion.velocity = chase_direction * enemy.speed;
		}
		break;
	case Sniper_State::Aiming: {
		motion.velocity = { 0, 0 };
		sniper.aim_timer -= elapsed_ms;
		float color_offset = ((50 - sniper.aim_timer) / 50) /4;
		vec3 color = { 0.2 - color_offset ,0.8 - color_offset,0.8 + color_offset };
		registry.colors.remove(sniperEntity);
		registry.colors.insert(sniperEntity, color);
		if (sniper.aim_timer <= 0) {
			sniper.state = Sniper_State::Shooting;
		}
		break;
	}
	case Sniper_State::Shooting: {
		vec2 bullet_direction = normalize(player_pos - motion.position);

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
		vec3 color = { 0.2,0.8,0.8 };
		registry.colors.remove(sniperEntity);
		registry.colors.insert(sniperEntity, color);
		create_enemy_bullet(renderer, motion.position, bullet_direction * 500.0f, angle_diff, 50, { 0,0,0 });
		sniper.state = Sniper_State::Avoiding;
		break;
	}
	}
}


void AISystem::updateCharger(Entity chargerEntity, vec2 chase_direction, 
	Minion& enemy, Motion& motion, float elapsed_ms, vec2 player_pos) {
	auto& charger = registry.chargers.get(chargerEntity);

	float distanceToPlayer = calculateDistance(motion.position, player_pos);


	switch (charger.state) {
	case Charger_State::Approaching:
		if (distanceToPlayer <= charger_aggro_range) {
			charger.state = Charger_State::Aiming;
			charger.aim_timer = charger_aim_time;
		}
		else {
			motion.velocity = chase_direction * enemy.speed;

		}
		break;
	case Charger_State::Aiming:
	{
		motion.velocity = { 0, 0 };

		charger.aim_timer -= elapsed_ms;
		float color_r = ((50 - charger.aim_timer) / 50) + 0.5;
		vec3 color = { color_r,0.2,0.2 };
		registry.colors.remove(chargerEntity);
		registry.colors.insert(chargerEntity, color);
		if (charger.aim_timer <= 0) {
			charger.aim_timer = 0;
			charger.state = Charger_State::Charging;
			charger.charge_direction = chase_direction;
		}
	}
		break;
	case Charger_State::Charging:
		motion.velocity = charger.charge_direction * charger_charge_speed * enemy.speed;
		charger.rest_timer += elapsed_ms * 2;
		if (charger.rest_timer >= charger_rest_time) {
			charger.rest_timer = 80;
			charger.state = Charger_State::Resting;
		}
		break;
	case Charger_State::Resting:
	{
		motion.velocity = chase_direction * enemy.speed * ((80 - charger.rest_timer) / 80);

		float color_r = (charger.rest_timer / 160) + 0.5;
		vec3 color = { color_r,0.2,0.2 };
		registry.colors.remove(chargerEntity);
		registry.colors.insert(chargerEntity, color);
		charger.rest_timer -= elapsed_ms;
		if (charger.rest_timer <= 0) {
			charger.state = Charger_State::Approaching;
		}
	}
		break;
	}
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
		
		create_enemy_bullet(renderer, motion.position, bullet_direction * 280.0f, angle_diff);

		
		shooter.time_since_last_shot_ms = static_cast<float>(distr(gen));
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
		else if(enemy.type == Enemy_TYPE::ROAMER){
			vec2 direction = normalize(motion.velocity);
			motion.velocity = direction * (original_speed);
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
		else if (enemy.type == Enemy_TYPE::CHARGER) {
			updateCharger(enemy_enitiy, chase_direction, enemy, motion, elapsed_ms, player_position);
		} 
		else if (enemy.type == Enemy_TYPE::SNIPER) {
			updateSniper(enemy_enitiy, chase_direction, enemy, motion, elapsed_ms, player_position);
		}
		else if (enemy.type == Enemy_TYPE::TANK) {
			updateTank(enemy_enitiy, chase_direction, enemy, motion, elapsed_ms, player_position);
		}
	}

	
}