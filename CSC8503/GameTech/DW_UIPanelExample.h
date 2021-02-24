#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"

#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"

#include "DW_UIRenderer.h"



class DW_UIPanelExample
{
public:
	DW_UIPanelExample();
	~DW_UIPanelExample();

private:
	DW_UIImage* m_bg;
	DW_UIImage* m_btn;
	DW_UIImage* m_close_btn;
	DW_UIText* m_text;

	DW_UIPanel* m_panel;
};

