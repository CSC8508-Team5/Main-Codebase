#include "DW_Rain.h"


DW_Rain::DW_Rain(const NCL::Maths::Vector3& g, const std::string& path) 
	:DW_ParticleSystem{g,path}
{
	SetRenderData();
}


DW_Rain::~DW_Rain() {

}

void DW_Rain::Update(const float dt) {
	for (std::vector<DW_Particle*>::iterator it = m_particles.begin(); it != m_particles.end();)
	{
		(*it)->SetVelocity(DW_Tools::ComputeVelocity((*it)->GetVelocity(), (*it)->GetAcceleration() + m_gravity, dt));
		(*it)->SetOriginPos((*it)->GetVelocity() * dt + (*it)->GetOriginPos());
		//(*it)->SetPosition(DW_Tools::ComputeEuler((*it)->GetPosition(), (*it)->GetVelocity(), (*it)->GetAcceleration() + m_gravity, dt));
		//(*it)->SetDeltMove(DW_Tools::ComputeVelocity((*it)->GetVelocity(), (*it)->GetAcceleration() + m_gravity, dt));
		(*it)->ModifyLifeSpan(-dt);
		if ((*it)->GetLifeSpan() <= 0.0f)
		{
			delete (*it);
			(*it) = nullptr;
			it = m_particles.erase(it);
		}
		else {
			++it;
		}
	}

	for (std::vector<DW_Particle*>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
		(*it)->Update(dt);

	m_time += dt;
	//std::cout << "time:" << m_time << "\n";

	//if (m_particles.size()<20)
	//{
		for (int i = 0; i <2; ++i)
			m_particles.push_back(new DW_Particle(DW_Particle::ParticleType::Rain, NCL::Maths::Vector3{}, m_time));
	//}
	
}


void DW_Rain::SetRenderData() {
	const int maxParticles = 10;
	GLfloat particlesContainter[maxParticles];

	for (int i = 0; i < maxParticles - 3; i += 3)
	{
		particlesContainter[i] = ((rand() % 1000) / 50.0f) - 1;
		particlesContainter[i + 1] = ((rand() % 1000) / 50.0f) - 1;
		particlesContainter[i + 2] = rand() % 100;
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particlesContainter), &particlesContainter, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void DW_Rain::Draw() {
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 10);
}