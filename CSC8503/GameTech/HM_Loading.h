
#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"

#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"

#include "DW_UIRenderer.h"



class HM_Loading
{
public:
	HM_Loading();
	~HM_Loading();

	void SetPanelActive(const bool flag) {
		m_panel1->SetPanelIsEnable(flag);
		m_isEnable1 = flag;
	}
	bool GetPanelIsEnable() { return m_isEnable1; }

private:
	DW_UIImage* m_bg1;


	DW_UIPanel* m_panel1;
	bool m_isEnable1;

	

};