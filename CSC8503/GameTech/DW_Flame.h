#pragma once

#include "DW_ParticleSystem.h"

#include "DW_Tools.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"


class DW_Flame:public DW_ParticleSystem
{
public:
	DW_Flame(const NCL::Maths::Vector3& g, const std::string& path);
	virtual ~DW_Flame();
	virtual void Update(const float dt);
	virtual void SetRenderData();

	void SetParticlePos(const NCL::Maths::Vector3& p) { m_particlePos = p; }
private:
	NCL::Maths::Vector3 m_particlePos;

	float DecideAlpha(const float num);
};

