#pragma once

#include "glad\glad.h"

#include <iostream>
#include <vector>

#include "DW_Particle.h"

class DW_ParticleSystem
{
public:
	DW_ParticleSystem(const NCL::Maths::Vector3& g);
	virtual ~DW_ParticleSystem();

	virtual void Update(const float dt);
private:
	GLuint m_VAO, m_Tex;
	std::vector<DW_Particle*> m_particles;
	NCL::Maths::Vector3 m_gravity;
};

