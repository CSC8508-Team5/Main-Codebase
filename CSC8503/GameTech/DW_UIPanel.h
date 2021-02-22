#pragma once

#include <map>

#include "DW_UIText.h"
#include "DW_UITextRenderer.h"


class DW_UIPanel
{
public:
	DW_UIPanel(const int priority=0);
	~DW_UIPanel();

	int GetRenderPriority() { return m_priority; }
	void SetRenderPriority(const int num) { m_priority = num; }

	void AddComponent(DW_UIbase* ui) { m_uiComponents.insert(std::pair<int, DW_UIbase*>{ui->GetRenderPriority(),ui}); }
	

	void Render();
private:
	int m_priority;

	std::multimap<int, DW_UIbase*> m_uiComponents;
	
};

