#include "DW_Flame.h"



DW_Flame::DW_Flame(const NCL::Maths::Vector3& g, const std::string& path)
	:DW_ParticleSystem{g, path}
{
	SetRenderData();
}

DW_Flame::~DW_Flame() {

}

void DW_Flame::Update(const float dt) {
	for (std::vector<DW_Particle*>::iterator it = m_particles.begin(); it != m_particles.end();)
	{
		(*it)->SetVelocity(DW_Tools::ComputeVelocity((*it)->GetVelocity(), (*it)->GetAcceleration() + m_gravity, dt));
		(*it)->SetPosition(DW_Tools::ComputeEuler((*it)->GetPosition(), (*it)->GetVelocity(), (*it)->GetAcceleration() + m_gravity, dt));
		(*it)->ModifyLifeSpan(-dt);
		if ((*it)->GetLifeSpan()<=0.0f)
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

	for (int i = 0; i < dt * 300; ++i) 
		m_particles.push_back(new DW_Particle());

	std::sort(this->m_particles.begin(), this->m_particles.end(), DW_Particle::SortParticles);
}

void DW_Flame::SetRenderData() {
	m_Tex = DW_Tools::LoadTexture(m_texPath.c_str());
	const int num = 300;
	NCL::Maths::Vector2 texture[num];
	NCL::Maths::Vector3 position[num];
	NCL::Maths::Vector4 colour[num];
	float high = 5, low = -5;
	for (int i = 0; i < num; i++) {
		float r1 = DW_Tools::RandomFloatBetween(-3, 3);
		float r2 = DW_Tools::RandomFloatBetween(-3, 3);;
		float r3 = DW_Tools::RandomFloatBetween(-3, 3);;
		position[i] = NCL::Maths::Vector3(r1, r2, r3);
		texture[i] = NCL::Maths::Vector2(r1, r3);
		if (r2 > 1.3f)
		colour[i] = NCL::Maths::Vector4(0.0, 0.0, 0.0, r1);
		else
		colour[i] = NCL::Maths::Vector4(1.0, 1.0, 0, r1);
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), &position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colour), &colour, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_texVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture), &texture, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

}