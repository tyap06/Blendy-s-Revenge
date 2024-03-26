#pragma once

#include <random>

//#include "Z:/cpp/SRC/lgl3/include/lgl3_includes.hpp"
//#include "Z:/cpp/SRC/lgl3/include/lglShader.hpp"

#include "common.hpp"
#include "world_init.hpp"


/*
emitter
*/
class SimpleEmitter
{
public:
	SimpleEmitter();
	~SimpleEmitter();

	void update(const float dt);

	void draw();

private:
	struct Particle
	{
		vec3 position;
		float lifetime;
	};
	std::vector< Particle > particles;

	float positions[400];

	GLuint vertexBuffer;
	GLuint positionBuffer;

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};



///*
//shader
//*/
//class SimpleParticleShader
//{
//public:
//	SimpleParticleShader();
//	~SimpleParticleShader();
//
//	void setUniformViewMatrix(const mat4& M_v);
//	void setUniformProjectionMatrix(const mat4& M_p);
//	void setUniformParticleSize(const float particleSize);
//
//private:
//	static const std::string VS;
//	static const std::string FS;
//
//	GLuint u_M_v;
//	GLuint u_M_p;
//	GLuint u_particleSize;
//};
//
//
//
///*
//controller
//*/
//class SimpleController
//{
//public:
//	SimpleController();
//	~SimpleController();
//
//	void draw(const float dt, const mat4 M_p, const mat4& M_v);
//
//private:
//	SimpleParticleShader* shader;
//
//	SimpleEmitter* emitter;
//};