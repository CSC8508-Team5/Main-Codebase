/*
Part of UI System code.

base class of ui component

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include <iostream>

#include "../../Common/Vector3.h"


class DW_UIbase
{
public:
	enum class UIType {
		NONE,
		TEXT,
		IMAGE	,
		HUD
	};
	DW_UIbase(const std::string& name, const UIType type, const NCL::Maths::Vector3& pos = NCL::Maths::Vector3{}, const NCL::Maths::Vector3& color = NCL::Maths::Vector3{1.0f,1.0f,1.0f}, const int priority = 0);
	virtual ~DW_UIbase();

	virtual void Update(const float dt);

	virtual void Render();

	UIType GetUIType() { return m_type; }
	int GetRenderPriority() { return m_renderPriority; }
	void SetRenderPriority(const int num) { m_renderPriority = num; }

	bool GetIsEnable() { return m_isEnable; }
	void SetIsEnable(const bool flag) { m_isEnable = flag; }

	bool operator <(const DW_UIbase& ui) {
		return this->m_renderPriority < ui.m_renderPriority;
	}

	bool operator >(const DW_UIbase& ui) {
		return this->m_renderPriority > ui.m_renderPriority;
	}

	void SetName(const std::string& name) { m_name = name; }
	std::string GetName() { return m_name; }

	void SetParentPanelName(const std::string& name) { m_ParentPanelName = name; }
	std::string GetParentPanelName() { return m_ParentPanelName; }


protected:
	UIType m_type;
	int m_renderPriority;
	bool m_isEnable;
	NCL::Maths::Vector3 m_position;
	NCL::Maths::Vector3 m_color;
	std::string m_name;

	std::string m_ParentPanelName;
};

