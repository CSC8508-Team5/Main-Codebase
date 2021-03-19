#include "DW_Particle.h"
#include "DW_Tools.h"


DW_Particle::DW_Particle(const ParticleType type, const NCL::Maths::Vector3& pos, const float time)
	:m_type{type}
{
	switch (m_type)
	{
	case ParticleType::Flame:
		InitFlameData(pos);
		break;
	case ParticleType::Rain:
		InitRainData(time);
		break;
	default:
		break;
	}
	
}


DW_Particle::~DW_Particle() {

}

void DW_Particle::Update(const float dt) {
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}

bool DW_Particle::SortParticles(DW_Particle* particleA, DW_Particle* particleB) {
	return particleA->m_position.z< particleB->m_position.z;
}

float DW_Particle::GenerateFlameRandomFloat() {
	return (rand() % 2000 - 1000) / 100.0f;
}

void DW_Particle::InitFlameData(const NCL::Maths::Vector3& pos) {
	//m_position = NCL::Maths::Vector3(-150.0f, 20.f, -30.0f);
	m_position = pos;
	m_lifespan = DW_Tools::RandomFloatBetween(1.4f, 1.9f);
	NCL::Maths::Vector3 newVelocity = NCL::Maths::Vector3(GenerateFlameRandomFloat(), GenerateFlameRandomFloat(), GenerateFlameRandomFloat());
	while (newVelocity.Length() > 10)
		newVelocity = NCL::Maths::Vector3(GenerateFlameRandomFloat(), GenerateFlameRandomFloat(), GenerateFlameRandomFloat());
	m_velocity = newVelocity;
	m_acceleration = NCL::Maths::Vector3(-0.8 * newVelocity.x, 0, -0.8 * newVelocity.z);
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}

void DW_Particle::InitRainData(const float time) {
	m_originPos = NCL::Maths::Vector3{};
	m_lifespan = DW_Tools::RandomFloatBetween(1.5f, 3.0f);
	m_velocity = NCL::Maths::Vector3(0.0f, -DW_Tools::RandomFloatBetween(1.5f, 2.5f), 0.0f);
	m_acceleration= NCL::Maths::Vector3(0.0f, -DW_Tools::RandomFloatBetween(0.0f, 0.5f), 0.0f);

	/*NCL::Maths::Vector3 temp{};
	float f = GenearteRainRandomFloat(NCL::Maths::Vector2{ 0.0f, 99.0f });
	NCL::Maths::Vector2 a = NCL::Maths::Vector2{ -f,-GenearteRainRandomFloat(NCL::Maths::Vector2{0.0f, 99.0f}) };
	NCL::Maths::Vector2 v = NCL::Maths::Vector2{ 0.1f,0.1f } + a * time;
	float x = std::min(std::max(v.x,-f), -0.001f );
	float y = std::min(std::max(v.y, -f), -0.001f);

	m_position = NCL::Maths::Vector3{ x * time,y * time,0.0f };*/

	m_position = NCL::Maths::Vector3{ DW_Tools::RandomFloatBetween(5.0f, 10.0f),DW_Tools::RandomFloatBetween(5.0f, 10.0f),0.0f };
}

float DW_Particle::GenearteRainRandomFloat(const NCL::Maths::Vector2& co) {
	float temp = NCL::Maths::Vector2::Dot(co, NCL::Maths::Vector2(12.9898, 78.233));
	temp = sin(temp)* 43758.5453f;
	temp = temp - floor(temp);
	return abs(temp);
}