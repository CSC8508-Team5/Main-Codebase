/*
Part of UI System code.

UI Image Component

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "DW_UIbase.h"
#include "DW_UIImageRenderer.h"

#include "DW_UIEventMgr.h"

class DW_UIImage:public DW_UIbase
{
public:
	struct Rect {
		float x;
		float y;
	};

	DW_UIImage(const std::string& name, char const* path, const Rect& rect = {0.2f,0.2f}, const NCL::Maths::Vector3& pos = NCL::Maths::Vector3{}, const NCL::Maths::Vector3& color = NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }, const int priority = 0);

	~DW_UIImage();

	virtual void Update(const float dt);

	virtual void Render();

private:
	Rect m_rect;
	GLuint m_texture;
	
};

