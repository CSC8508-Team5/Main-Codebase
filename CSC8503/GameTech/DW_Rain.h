#pragma once
#include "DW_ParticleSystem.h"
class DW_Rain : public DW_ParticleSystem
{
public:
	DW_Rain(const NCL::Maths::Vector3& g = NCL::Maths::Vector3{}, const std::string& path = "");
	virtual ~DW_Rain();

	virtual void Update(const float dt);
	virtual void SetRenderData();

	void Draw();
private:

};

