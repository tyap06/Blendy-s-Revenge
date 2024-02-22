#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float BLENDY_BB_WIDTH = 0.6f * 165.f;
const float BLENDY_BB_HEIGHT = 0.6F * 300.f;

// the player (Blendy)
Entity createBlendy(RenderSystem* renderer, vec2 pos);


