#include "DW_Rain.h"


DW_Rain::DW_Rain(const NCL::Maths::Vector3& g, const std::string& path) 
	:DW_ParticleSystem{g,path}
{
	SetRenderData();
}


DW_Rain::~DW_Rain() {

}

void DW_Rain::Update(const float dt) {

}


void DW_Rain::SetRenderData() {
	const int maxParticles = 3000;
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
	glDrawArrays(GL_POINTS, 0, 3000);
}