/*
Part of UI System code.

A UIPanel can hold many ui component

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include <map>

#include "DW_UIbase.h"



class DW_UIPanel
{
public:
	DW_UIPanel(const std::string& name, const int priority=0);
	~DW_UIPanel();

	int GetRenderPriority() { return m_priority; }
	void SetRenderPriority(const int num) { m_priority = num; }

	void AddComponent(DW_UIbase* ui);
	
	void Update(const float dt);

	void Render();

	std::string GetPanelName() { return m_name; }
	void SetPanelName(const std::string& name) { m_name = name; }

	bool GetPanelIsEnable() { return m_isEnable; }
	void SetPanelIsEnable(const bool flag) { m_isEnable = flag; }
private:
	int m_priority;
	bool m_isEnable;
	std::multimap<int, DW_UIbase*> m_uiComponents;

	std::string m_name;
	
};

