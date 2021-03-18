#include "DW_ParticleSystem.h"



DW_ParticleSystem::DW_ParticleSystem(const NCL::Maths::Vector3& g, const std::string& path)
	:m_gravity{ g }, m_texPath{path}
{
	
}


DW_ParticleSystem::~DW_ParticleSystem() {

}

void DW_ParticleSystem::Update(const float dt) {}

void DW_ParticleSystem::SetRenderData() {}