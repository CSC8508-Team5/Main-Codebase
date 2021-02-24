#include "DW_UIPanel.h"


DW_UIPanel::DW_UIPanel(const int priority) :m_isEnable{true} {
	
}


DW_UIPanel::~DW_UIPanel() {

}

void DW_UIPanel::Update(const float dt) {
	if (!m_isEnable)
	{
		return;
	}
	for (std::multimap<int, DW_UIbase*>::reverse_iterator rit = m_uiComponents.rbegin(); rit != m_uiComponents.rend(); rit++)
	{
		rit->second->Update(dt);
	}
}

void DW_UIPanel::Render() {
	if (!m_isEnable)
	{
		return;
	}
	for (std::multimap<int,DW_UIbase*>::iterator it = m_uiComponents.begin(); it != m_uiComponents.end(); it++)
	{
		it->second->Render();
	}
}