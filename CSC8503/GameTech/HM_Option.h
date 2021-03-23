#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"

#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"

#include "DW_UIRenderer.h"
#include "HM_StartMenu.h"
#include "../CSC8503Common/AudioSystem.h"

#include "../CSC8503Common/LanguageManager.h"

class HM_Option
{
public:
	HM_Option(NCL::LanguageManager* lm);
	~HM_Option();

	void SetPanelActive(const bool flag) {
		m_panel->SetPanelIsEnable(flag);
		m_isEnable = flag;
	}
	bool GetPanelIsEnable() { return m_isEnable; }
	double volume;
private:
	DW_UIImage* m_bg;

	DW_UIImage* VolumeUp_btn;
	DW_UIImage* VolumeDown_btn;
	DW_UIImage* Volume_btn;
	DW_UIImage* Close_btn;

	DW_UIText* Option_text; 
	DW_UIText* Close_text;

	DW_UIText* Volume_text;

	DW_UIText* VolumeUp_text;
	DW_UIText* VolumeDown_text;
	

	DW_UIPanel* m_panel;

	bool m_isEnable = false;
	void ClickFunc(const std::string& str);

	NCL::LanguageManager* lang;
};

