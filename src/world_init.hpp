#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"


// ENTITY TEXTURE CONSTANTS

const float BLENDY_BB_WIDTH = 0.1f * 771.f;
const float BLENDY_BB_HEIGHT = 0.1F * 1511.f;
const float MINION_BB_WIDTH = 0.2f * 346.f;
const float MINION_BB_HEIGHT = 0.2f * 593.f;
const float BACKGROUND_BB_WIDTH = 0.85f * 2131.f;
const float BACKGROUND_BB_HEIGHT = 0.85f * 1563.f;
const float DIRECTIONAL_LIGHT_BB_WIDTH = 0.1f * 512.f;
const float DIRECTIONAL_LIGHT_BB_HEIGHT = 0.1f * 512.f;


Entity createBullet(RenderSystem* renderer, vec2 pos, vec2 velocity);

// the background
Entity create_background(RenderSystem* renderer, vec2 pos, vec2 bounds);

// the player (Blendy)
Entity create_blendy(RenderSystem* renderer, vec2 pos, vec2 bounds);

// the directional light for Blinn-Phong
Entity create_directional_light(RenderSystem* renderer, vec2 pos, vec2 bounds);

// the minions
Entity create_minion(RenderSystem* renderer, vec2 position, vec2 bounds);

