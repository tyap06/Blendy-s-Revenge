#include "SimpleParticles.hpp"


SimpleEmitter::SimpleEmitter()
{

}



SimpleEmitter::~SimpleEmitter()
{
	glDeleteBuffers(1, &this->vertexBuffer);
	glDeleteBuffers(1, &this->positionBuffer);

	gl_has_errors();
}

void SimpleEmitter::update(const float& dt)
{
	for (uint i = 0; i < this->particles.size(); ++i)
	{
		// subtract from the particles lifetime
		this->particles[i].lifetime -= dt;

		auto& current_angle = this->particles[i].angle;
		float velocity_magnitude = this->particles[i].velocity_magnitude; // The magnitude of the velocity vector of the particles
		float x_velocity = velocity_magnitude * cos(glm::radians(current_angle));
		float y_velocity = velocity_magnitude * sin(glm::radians(current_angle));

		// move the particle outwards depending on the delta time
		this->particles[i].position += dt * vec3(x_velocity, y_velocity, 0.0f);

		// update the position buffer
		this->positions[i * 4 + 0] = this->particles[i].position[0];
		this->positions[i * 4 + 1] = this->particles[i].position[1];
		this->positions[i * 4 + 2] = this->particles[i].position[2];
		this->positions[i * 4 + 3] = this->particles[i].lifetime;
	}
}



void SimpleEmitter::draw()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(4);

	gl_has_errors();

	// update the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->particles.size() * 4 * sizeof(float), this->positions);

	gl_has_errors();

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	gl_has_errors();

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(4, 1);

	gl_has_errors();

	// draw triangles
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, this->particles.size());

	gl_has_errors();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(4);

	gl_has_errors();
}

void SimpleEmitter::seed_random_number_generator()
{
	auto time_since_beginning = glfwGetTime();
	auto new_seed = time_since_beginning * 10000000;
	rng.seed(new_seed);
}

void SimpleEmitter::set_emitter_uniforms()
{
	// Add any emitter uniforms that you want to configure here later
}

void SimpleEmitter::set_particle_attributes(
	unsigned int i,
	const vec2& emitter_position = vec2{ window_width_px / 2, window_height_px / 2 },
	const float& base_lifetime = (float) 10.f,
	const float centre_offset = 30.f,
	const float& velocity_magnitude = 0.05f,
	const vec3& start_color = vec3{1.0,0.0,0.0},
	const vec3& end_color = vec3{ 1.0,1.0,1.0 }
	)
{
	seed_random_number_generator();

	//constexpr float base_lifetime = 10.f;
	//const vec2 emitter_position = vec2{ window_width_px / 2, window_height_px / 2 };
	//constexpr float centre_offset = 30.f; // per emitter value

	const float lifetime = base_lifetime;
	const float random_angle = uniform_dist(rng) * 360.f;
	const float random_position_x = emitter_position.x + (uniform_dist(rng) - 0.5f) * centre_offset; // [width/2-30,width/2+30];
	const float random_position_y = emitter_position.y + (uniform_dist(rng) - 0.5f) * centre_offset; // [height/2-30,height/2+30];

	this->particles[i].position = vec3(random_position_x, random_position_y, 1.0);
	this->particles[i].lifetime = lifetime;
	this->particles[i].angle = random_angle;
	this->particles[i].start_color = start_color;
	this->particles[i].end_color = end_color;
	this->particles[i].velocity_magnitude = velocity_magnitude;
}

void SimpleEmitter::init(
	const vec2& emitter_position,
	const float& base_lifetime,
	const float& centre_offset,
	const float& outward_velocity_magnitude
)
{
	const unsigned int NUMBER_OF_PARTICLES = 100;
	// create 100 particles
	this->particles.resize(NUMBER_OF_PARTICLES);
	for (uint i = 0; i < this->particles.size(); ++i)
	{
		set_particle_attributes(i, emitter_position, base_lifetime, centre_offset, outward_velocity_magnitude);
		set_emitter_uniforms();
	}

	// create a vertex and position buffer
	glGenBuffers(1, &this->vertexBuffer);
	glGenBuffers(1, &this->positionBuffer);

	gl_has_errors();

	// fill the vertex buffer
	std::vector< float > vertices;
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	vertices.push_back(1.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	vertices.push_back(0.0f);
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);

	vertices.push_back(1.0f);
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	gl_has_errors();

	// fill the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->particles.size() * 4 * sizeof(float), this->positions, GL_DYNAMIC_DRAW);

	gl_has_errors();
}