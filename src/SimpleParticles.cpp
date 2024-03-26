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



void SimpleEmitter::update(const float dt)
{
	for (uint i = 0; i < this->particles.size(); ++i)
	{
		// subtract from the particles lifetime
		this->particles[i].lifetime -= dt;

		// if the lifetime is below 0 respawn the particle
		if (this->particles[i].lifetime <= 0.0f)
		{
			set_particle_attributes(i);
		}

		// move the particle down depending on the delta time
		this->particles[i].position -= vec3(0.0f, dt * 2.0f, 0.0f);

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

void SimpleEmitter::set_particle_attributes(uint i)
{
	// give every particle a random position
	//const float random_position_x = uniform_dist(rng) * 2.f - 1.f; // [-1,1]
	//const float random_position_y = uniform_dist(rng) * 2.f - 1.f; // [-1,1]
	//const float random_position_z = uniform_dist(rng) * 2.f - 1.f; // [-1,1]
	//const float random_lifetime = uniform_dist(rng) + 1.f; // [1,2]

	constexpr float centre_offset = 60.f;
	const float random_position_x = window_width_px / 2 + (uniform_dist(rng) - 0.5f) * centre_offset; // [width/2-30,width/2+30];
	const float random_position_y = window_height_px / 2 + (uniform_dist(rng) - 0.5f) * centre_offset; // [height/2-30,height/2+30];
	//const float random_position_x = 0.f;
	//const float random_position_y = 0.f;
	constexpr float random_position_z = 10.0;
	const float random_lifetime = uniform_dist(rng) + 1.f; // [1,2]

	this->particles[i].position = vec3(random_position_x, random_position_y, random_position_z);
	this->particles[i].lifetime = random_lifetime;
}

void SimpleEmitter::init()
{
	// create 100 particles
	this->particles.resize(100);
	for (uint i = 0; i < this->particles.size(); ++i)
	{
		set_particle_attributes(i);
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