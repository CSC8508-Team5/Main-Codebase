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
private:
	//const int m_VertexNum{ 300 };
};

