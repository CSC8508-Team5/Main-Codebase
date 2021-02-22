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

protected:
	std::multimap<int,DW_UIPanel> m_panels;
};

