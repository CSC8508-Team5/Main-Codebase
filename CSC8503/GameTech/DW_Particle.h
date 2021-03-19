#pragma once

#include <cmath>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"


class DW_Particle
{
public:
	enum ParticleType {
		Flame,
		Rain
	};

	DW_Particle(const ParticleType type, const NCL::Maths::Vector3& pos, const float time);
	~DW_Particle();

	NCL::Maths::Vector3 GetPosition() { return m_position; }
	void SetPosition(const NCL::Maths::Vector3& pos) { m_position = pos; }

	NCL::Maths::Vector3 GetVelocity() { return m_velocity; }
	void SetVelocity(const NCL::Maths::Vector3& v) { m_velocity = v; }

	NCL::Maths::Vector3 GetAcceleration() { return m_acceleration; }
	NCL::Maths::Matrix4 GetModelMatrix() { return m_modelMatrix; }

	NCL::Maths::Vector3 GetDeltMove() { return m_deltMove; }
	void SetDeltMove(const NCL::Maths::Vector3& d) {m_deltMove=d; }

	NCL::Maths::Vector3 GetOriginPos() { return m_originPos; }
	void SetOriginPos(const NCL::Maths::Vector3& o) { m_originPos = o; }

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

	NCL::Maths::Vector3 m_deltMove;
	NCL::Maths::Vector3 m_originPos;

	float GenerateFlameRandomFloat();
	void InitFlameData(const NCL::Maths::Vector3& pos);
	void InitRainData(const float time);
	float GenearteRainRandomFloat(const NCL::Maths::Vector2& co);
};

