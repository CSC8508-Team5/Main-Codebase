/*
Part of UI System code.

UI Text Component

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include <string>

#include "DW_UIbase.h"
#include "DW_UITextRenderer.h"



class DW_UIText :public DW_UIbase
{
public:

	DW_UIText(const std::string& name, const std::string& str="",const float scale=0.3f ,const NCL::Maths::Vector3& pos = NCL::Maths::Vector3{}, const NCL::Maths::Vector3& color = NCL::Maths::Vector3{ 1.0f,1.0f,1.0f },const int priority=0);
	virtual ~DW_UIText();

	virtual void Update(const float dt);

	virtual void Render();

	void SetText(const std::string& str) { m_text = str; }
	std::string GetText() { return m_text; }
private:
	std::string m_text;
	float m_scale;
};

