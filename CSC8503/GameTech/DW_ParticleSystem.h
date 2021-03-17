#pragma once

#include "glad\glad.h"

#include <iostream>
#include <vector>
#include <string>

#include "DW_Particle.h"

class DW_ParticleSystem
{
public:
	DW_ParticleSystem(const NCL::Maths::Vector3& g, const std::string& path);
	virtual ~DW_ParticleSystem();

	virtual void Update(const float dt);
	virtual void SetRenderData();

	GLuint GetVAO() { return m_VAO; }
	GLuint GetTexture() { return m_Tex; }

	std::vector<DW_Particle*>::iterator GetParticlesBegin() { return m_particles.begin(); }
	std::vector<DW_Particle*>::iterator GetParticlesEnd() { return m_particles.end(); }

	/*std::pair<std::vector<DW_Particle*>::iterator, std::vector<DW_Particle*>::iterator> GetParticlesPointer() {
		return std::pair<std::vector<DW_Particle*>::iterator, std::vector<DW_Particle*>::iterator>(m_particles.begin(), m_particles.end());
	}*/

protected:
	GLuint m_VAO, m_Tex;
	GLuint m_posVBO, m_colorVBO, m_texVBO;
	std::vector<DW_Particle*> m_particles;
	NCL::Maths::Vector3 m_gravity;
	std::string m_texPath;
};

