/*
Part of UI System code.

Manage all UIPanels

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include <iostream>
#include <map>

#include "DW_Singleton.h"
#include "DW_UIPanel.h"

class DW_UIRenderer:public DW_Singleton<DW_UIRenderer>
{
public:
	DW_UIRenderer(token) {
		std::cout << "DW_UIRenderer Init!\n";
	}

	~DW_UIRenderer() {
		std::cout << "DW_UIRenderer delete!\n";
	}

	DW_UIRenderer(const DW_UIRenderer&) = delete;
	DW_UIRenderer& operator =(const DW_UIRenderer&) = delete;

	void Render();

	void AddPanel(DW_UIPanel* panel) { m_panels.insert(std::pair<int, DW_UIPanel*>(panel->GetRenderPriority(), panel)); }

protected:
	std::multimap<int,DW_UIPanel*> m_panels;
};

