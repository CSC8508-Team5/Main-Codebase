
/*
Part of UI System code.

An example of how to create an UIPanel

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"

#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"

#include "DW_UIRenderer.h"
#include "HM_StartMenu.h"


class  HM_PauseMenu
{
public:
	HM_PauseMenu();
	~HM_PauseMenu();

	void SetPanelActive(const bool flag) { 
		m_panel->SetPanelIsEnable(flag); 
		m_isEnable = flag;
	}
	bool GetPanelIsEnable() { return m_isEnable; }


private:
	DW_UIImage* m_bg;
	
	DW_UIImage* Continue_btn;
	DW_UIImage* Back_btn;

	DW_UIImage* m_close_btn;
	DW_UIText* PauseMenu_text;

	DW_UIText* Continue_text;
	DW_UIText* Back_text;
	

	DW_UIPanel* m_panel;

	HM_StartMenu* StartMenu;
	bool m_isEnable;
	void ClickFunc(const std::string& str);

};

