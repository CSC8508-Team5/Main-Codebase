/*
Part of UI System code.

deal with capture mouse click and dispatch event

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include <iostream>
#include <functional>

#include "eventdispatcher.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Window.h"

#include "DW_Singleton.h"
class DW_UIEventMgr :public DW_Singleton<DW_UIEventMgr>
{
public:
	DW_UIEventMgr(token) {
		std::cout << "DW_UIEventMgr Init!\n";

	}

	~DW_UIEventMgr() {
		std::cout << "DW_UIEventMgr delete!\n";
	}

	DW_UIEventMgr(const DW_UIEventMgr&) = delete;
	DW_UIEventMgr& operator =(const DW_UIEventMgr&) = delete;


	bool CheckUIImageClick(const float scaleX,const float scaleY, const NCL::Maths::Vector3& center);

	void SetHasFindTarget(const bool flag) { m_hasFindTarget = flag; }

	bool GetHasFindTarget() { return m_hasFindTarget; }

	void RegisterUIEvent(const std::string& type, const std::function<void(const std::string&)>& func);

	void TriggerUIEvent(const std::string& type, const std::string& para);

private:
	const float m_aspect{ 0.5625f };
	bool m_hasFindTarget{false};

	//event type is the name of the panel, and function's parameter is the name of the component
	eventpp::EventDispatcher<std::string, void(std::string)> m_uiEventDispatcher;

};

