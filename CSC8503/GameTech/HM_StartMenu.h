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

#include "../CSC8503Common/LanguageManager.h"

class HM_StartMenu
{
public:
	HM_StartMenu(NCL::LanguageManager* lm);
	~HM_StartMenu();

	void SetPanelActive(const bool flag) {
		m_panel->SetPanelIsEnable(flag);
		m_isEnable = flag;
	}
	bool GetPanelIsEnable() { return m_isEnable; }

	int GetInput() { return input; }
	void ResetInput() { input = 0; }
private:
	DW_UIImage* m_bg;

	DW_UIImage* Solo_btn;
	DW_UIImage* Duo_btn;
	DW_UIImage* Quit_btn;

	DW_UIImage* m_close_btn;
	DW_UIText* GameName_text;

	DW_UIText* Solo_text;
	DW_UIText* Duo_text;
	DW_UIText* Quit_text;

	DW_UIPanel* m_panel;
	bool m_isEnable;

	void ClickFunc(const std::string& str);

	int input = 0;

	NCL::LanguageManager* lang;
};

