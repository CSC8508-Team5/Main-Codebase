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

	for (int i = 0; i < dt * 1000; ++i) 
		m_particles.push_back(new DW_Particle(m_particlePos));

	std::sort(this->m_particles.begin(), this->m_particles.end(), DW_Particle::SortParticles);
}

void DW_Flame::SetRenderData() {
	m_Tex = DW_Tools::LoadTexture(m_texPath.c_str());

	const int num = 3000;
	NCL::Maths::Vector2 texture[num];
	NCL::Maths::Vector3 position[num];
	NCL::Maths::Vector4 colour[num];
	float high = 5, low = -5;
	for (int i = 0; i < num; i++) {
		float r1 = DW_Tools::RandomFloatBetween(high, low); 
		float r2 = DW_Tools::RandomFloatBetween(high, low);
		float r3 = DW_Tools::RandomFloatBetween(high, low);

		position[i] = NCL::Maths::Vector3(r1, r2, r3);
		texture[i] = NCL::Maths::Vector2(r1, r3);
		if (r2 > 2.3f)
		colour[i] = NCL::Maths::Vector4(0.0, 0.0, 0.0, r1);
		else
		colour[i] = NCL::Maths::Vector4(1.0, 1.0, 0, r1);
	}
	

	//NCL::Maths::Vector2 texture[4][4];
	//NCL::Maths::Vector3 position[4][4];
	//NCL::Maths::Vector4 colour[4][4];

	//float high = 2, low = -2;
	//for (int i = 0; i < 4; i++) {
	//	float t1 = DW_Tools::RandomFloatBetween(0.f, 0.225f);
	//	float t2 = DW_Tools::RandomFloatBetween(0.f, 0.225f);
	//	float t3 = DW_Tools::RandomFloatBetween(0.f, 0.225f);
	//	texture[i][0] = NCL::Maths::Vector2(0.f, t1);
	//	texture[i][1] = NCL::Maths::Vector2(0.f, 0.f);
	//	texture[i][2] = NCL::Maths::Vector2(t2, t2);
	//	texture[i][3] = NCL::Maths::Vector2(t3, 0.f);
	//	for (int j = 0; j < 4; j++) {
	//		float r1 = DW_Tools::RandomFloatBetween(low, high);
	//		float r2 = DW_Tools::RandomFloatBetween(low, high);
	//		//float r3 = DW_Tools::RandomFloatBetween(0, 1);
	//		float r4 = DW_Tools::RandomFloatBetween(-40.f, -50.f);
	//		position[i][j] = NCL::Maths::Vector3(r1, r2, r4);
	//		colour[i][j] = NCL::Maths::Vector4(1.0, 1.0, 0.0, DecideAlpha(r4));
	//	}
	//}

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

float DW_Flame::DecideAlpha(const float _number) {
	float toReturn = 1.f;
	if (_number <= -40.f && _number > -41.f)
		toReturn = 0.9f;
	else if (_number <= -41.f && _number > -42.f)
		toReturn = 0.8f;
	else if (_number <= -42.f && _number > -43.f)
		toReturn = 0.7f;
	else if (_number <= -43.f && _number > -44.f)
		toReturn = 0.6f;
	else if (_number <= -44.f && _number > -45.f)
		toReturn = 0.5f;
	else if (_number <= -45.f && _number > -46.f)
		toReturn = 0.4f;
	else if (_number <= -46.f && _number > -47.f)
		toReturn = 0.3f;
	else if (_number <= -47.f && _number > -48.f)
		toReturn = 0.2f;
	else if (_number <= -48.f && _number > -49.f)
		toReturn = 0.1f;
	else if (_number <= -49.f && _number >= -50.f)
		toReturn = 0.05f;
	return toReturn;
}