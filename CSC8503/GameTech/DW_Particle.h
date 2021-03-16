#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"


class DW_Particle
{
public:
	DW_Particle();
	~DW_Particle();

	NCL::Maths::Vector3 GetPosition() { return m_position; }

	void Update(const float dt);

	static bool SortParticles(const DW_Particle& particleA, const DW_Particle& particleB);
private:
	float m_lifespan;
	NCL::Maths::Vector3 m_position;
	NCL::Maths::Vector3 m_velocity;
	NCL::Maths::Vector3 m_acceleration;
	NCL::Maths::Matrix4 m_modelMatrix;

	float GenerateRandomFloat();
	
};

