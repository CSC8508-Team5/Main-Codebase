/*
Part of UI System code.

a HUD instance in the scene

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"





class DW_UIHUD
{
public:
	DW_UIHUD(char const* path, const NCL::Maths::Vector2& scale, const NCL::Maths::Vector3& offset);
	~DW_UIHUD();

	unsigned int GetTexture() { return m_texture; }
	NCL::Maths::Vector2 GetScale() { return m_scale; }
	NCL::Maths::Vector3 GetPosOffset() { return m_offset; }

private:
	unsigned int m_texture;
	NCL::Maths::Vector2 m_scale;
	NCL::Maths::Vector3 m_offset;
};

