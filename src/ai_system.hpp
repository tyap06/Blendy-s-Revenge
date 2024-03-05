#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

class AISystem
{
public:
	void init(RenderSystem* renderer_arg);
	void step(float elapsed_ms);
private:
	void shoot(float time);
	RenderSystem* renderer;
};



