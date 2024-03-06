#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

enum class ShooterState {
    Approach,
    Dodge,
};



class AISystem
{
public:
	void init(RenderSystem* renderer_arg);
	void step(float elapsed_ms);
private:
	void shoot(Entity shooterEntity, const vec2& playerPosition, float elapsed_ms);
	RenderSystem* renderer;
};



