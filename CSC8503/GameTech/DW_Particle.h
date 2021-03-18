#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"


class DW_Particle
{
public:
	enum ParticleType {
		Flame,
		Rain
	};

	DW_Particle(const ParticleType type, const NCL::Maths::Vector3& pos);
	~DW_Particle();

	NCL::Maths::Vector3 GetPosition() { return m_position; }
	void SetPosition(const NCL::Maths::Vector3& pos) { m_position = pos; }

	NCL::Maths::Vector3 GetVelocity() { return m_velocity; }
	void SetVelocity(const NCL::Maths::Vector3& v) { m_velocity = v; }

	NCL::Maths::Vector3 GetAcceleration() { return m_acceleration; }
	NCL::Maths::Matrix4 GetModelMatrix() { return m_modelMatrix; }

	void ModifyLifeSpan(const float dt) { m_lifespan += dt; }
	float GetLifeSpan() { return m_lifespan; }

	void Update(const float dt);
	static bool SortParticles(DW_Particle* particleA, DW_Particle* particleB);
private:
	ParticleType m_type;
	float m_lifespan;
	NCL::Maths::Vector3 m_position;
	NCL::Maths::Vector3 m_velocity;
	NCL::Maths::Vector3 m_acceleration;
	NCL::Maths::Matrix4 m_modelMatrix;

	float GenerateRandomFloat();
	void InitFlameData(const NCL::Maths::Vector3& pos);
	void InitRainData();
};

