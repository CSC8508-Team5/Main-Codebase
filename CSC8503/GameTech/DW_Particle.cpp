#include "DW_Particle.h"
#include "DW_Tools.h"


DW_Particle::DW_Particle(const NCL::Maths::Vector3& pos) {
	InitFlameData(pos);
}


DW_Particle::~DW_Particle() {

}

void DW_Particle::Update(const float dt) {
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}

bool DW_Particle::SortParticles(DW_Particle* particleA, DW_Particle* particleB) {
	return particleA->m_position.z< particleB->m_position.z;
}

float DW_Particle::GenerateRandomFloat() {
	return (rand() % 2000 - 1000) / 100.0f;
}

void DW_Particle::InitFlameData(const NCL::Maths::Vector3& pos) {
	//m_position = NCL::Maths::Vector3(-150.0f, 20.f, -30.0f);
	m_position = pos;
	m_lifespan = DW_Tools::RandomFloatBetween(1.4f, 1.9f);
	NCL::Maths::Vector3 newVelocity = NCL::Maths::Vector3(GenerateRandomFloat(), GenerateRandomFloat(), GenerateRandomFloat());
	while (newVelocity.Length() > 10)
		newVelocity = NCL::Maths::Vector3(GenerateRandomFloat(), GenerateRandomFloat(), GenerateRandomFloat());
	m_velocity = newVelocity;
	m_acceleration = NCL::Maths::Vector3(-0.8 * newVelocity.x, 0, -0.8 * newVelocity.z);
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}