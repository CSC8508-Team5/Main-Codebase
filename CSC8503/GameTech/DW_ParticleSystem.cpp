#include "DW_ParticleSystem.h"



DW_ParticleSystem::DW_ParticleSystem(const NCL::Maths::Vector3& g) 
	:m_gravity{g}
{

}


DW_ParticleSystem::~DW_ParticleSystem() {

}

void DW_ParticleSystem::Update(const float dt) {}