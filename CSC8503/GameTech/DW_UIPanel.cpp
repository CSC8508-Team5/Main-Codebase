#include "DW_UIPanel.h"


DW_UIPanel::DW_UIPanel(const int priority) {
	
}


DW_UIPanel::~DW_UIPanel() {

}

void DW_UIPanel::Render() {
	for (std::multimap<int,DW_UIbase*>::iterator it = m_uiComponents.begin(); it != m_uiComponents.end(); it++)
	{
		it->second->Render();
	}

	
}