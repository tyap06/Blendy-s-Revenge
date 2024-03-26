#pragma once

#include <random>
#include "common.hpp"

class SimpleEmitter
{
public:
	SimpleEmitter();
	~SimpleEmitter();

	void update(const float dt);
	void draw();
	void init();

private:
	struct Particle
	{
		vec3 position;
		float lifetime;
	};

	std::vector<Particle> particles;

	float positions[400];

	GLuint vertexBuffer;
	GLuint positionBuffer;

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Helper Functions
	void set_particle_attributes(uint i);
};