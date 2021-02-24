#include "DW_UIRenderer.h"


void DW_UIRenderer::Update(const float dt) {
	for (std::multimap<int, DW_UIPanel*>::reverse_iterator rit = m_panels.rbegin(); rit != m_panels.rend(); rit++)
	{
		rit->second->Update(dt);
	}
	ClearUpdateState();
}

void DW_UIRenderer::Render() {
	for (std::multimap<int,DW_UIPanel*>::iterator it = m_panels.begin(); it !=m_panels.end(); it++)
	{
		it->second->Render();
	}
}

void DW_UIRenderer::ClearUpdateState() {
	DW_UIEventMgr::get_instance().SetHasFindTarget(false);
}