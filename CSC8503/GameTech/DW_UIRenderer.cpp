#include "DW_UIRenderer.h"


void DW_UIRenderer::Render() {
	for (std::multimap<int,DW_UIPanel*>::iterator it = m_panels.begin(); it !=m_panels.end(); it++)
	{
		it->second->Render();
	}
}