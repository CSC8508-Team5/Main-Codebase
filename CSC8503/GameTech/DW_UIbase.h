#pragma once

#include <iostream>

//#include "../../Common/Vector3.h"


class DW_UIbase
{
public:
	enum UIType {
		NONE,
		TEXT,
		IMAGE	,
		HUD
	};
	DW_UIbase(const UIType type);
	virtual ~DW_UIbase();

	virtual void Update(const float dt) = 0;

	virtual void Render() = 0;

	UIType GetUIType() { return m_type; }
	int GetRenderPriority() { return m_renderPriority; }
	void SetRenderPriority(const int num) { m_renderPriority = num; }

protected:
	UIType m_type;
	int m_renderPriority;
	/*NCL::Maths::Vector3 m_position;*/

};

