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

#include "../CSC8503Common/LanguageManager.h"


class  HM_Lose
{
public:
	HM_Lose(NCL::LanguageManager* lm);
	~HM_Lose();
	bool IfRestart() { return restart; }
	void SetRestart(const bool flag) {
		restart = flag;
	}

	void SetPanelActive(const bool flag) {
		m_panel->SetPanelIsEnable(flag);
		m_isEnable = flag;
	}
	bool GetPanelIsEnable() { return m_isEnable; }

	int GetInput() { return input; }
	void ResetInput() { input = 0; }
private:
	DW_UIImage* m_bg;
	DW_UIImage* m_lose;

	DW_UIImage* PlayAgain_btn;
	DW_UIImage* Back_btn;


	DW_UIText* LoseScreen_text;

	DW_UIText* PlayAgain_text;
	DW_UIText* Back_text;

	DW_UIText* Score_text;

	DW_UIPanel* m_panel;
	HM_StartMenu* StartMenu;
	bool m_isEnable;
	bool restart;
	void ClickFunc(const std::string& str);

	NCL::LanguageManager* lang;

	int input = 0;
};

