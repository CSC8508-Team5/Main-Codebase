#include "DW_Particle.h"
#include "DW_Tools.h"


DW_Particle::DW_Particle() {
	m_position = NCL::Maths::Vector3(0.0f, -10.f, 0.f);
	m_lifespan = DW_Tools::RandomFloatBetween(1.4f, 1.9f);
	NCL::Maths::Vector3 newVelocity = NCL::Maths::Vector3(GenerateRandomFloat(), GenerateRandomFloat(), GenerateRandomFloat());
	while (newVelocity.Length() > 10)
		newVelocity = NCL::Maths::Vector3(GenerateRandomFloat(), GenerateRandomFloat(), GenerateRandomFloat());
	m_velocity = newVelocity;
	m_acceleration = NCL::Maths::Vector3(-0.8 * newVelocity.x, 0, -0.8 * newVelocity.z);
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}


DW_Particle::~DW_Particle() {

}

void DW_Particle::Update(const float dt) {
	m_modelMatrix = NCL::Maths::Matrix4::Translation(m_position);
}

bool DW_Particle::SortParticles(const DW_Particle& particleA, const DW_Particle& particleB) {
	return  particleA.m_position.z< particleB.m_position.z;//ascending
}

float DW_Particle::GenerateRandomFloat() {
	return (rand() % 2000 - 1000) / 100.0f;
}