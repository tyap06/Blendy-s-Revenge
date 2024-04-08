#pragma once

#include <random>
#include "common.hpp"

class SimpleEmitter
{
public:
	SimpleEmitter();
	~SimpleEmitter();

	void update(const float& dt);
	void draw();
	void seed_random_number_generator();
	void set_emitter_uniforms(
		
	);
	void init(
		const vec2& emitter_position,
		const float& base_lifetime,
		const float& centre_offset, 
		const float& outward_velocity_magnitude
	);

private:
	struct Particle
	{
		vec3 position;
		float angle;
		float lifetime;
		float velocity_magnitude;
		vec3 start_color;
		vec3 end_color;
	};

	std::vector<Particle> particles;

	float positions[400];

	GLuint vertexBuffer;
	GLuint positionBuffer;

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Helper Functions
	void set_particle_attributes(
		unsigned i,
		const vec2& emitter_position,
		const float& base_lifetime,
		const float centre_offset,
		const float& velocity_magnitude,
		const vec3& end_color, const vec3& start_color
	);
};