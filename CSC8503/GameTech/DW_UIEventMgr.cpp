#include "DW_UIEventMgr.h"


bool DW_UIEventMgr::CheckUIImageClick(const float scaleX, const float scaleY, const NCL::Maths::Vector3& center) {
	/*NCL::Maths::Vector2 mousePos = NCL::Window::GetMouse()->GetAbsolutePosition();
	mousePos.y = 720.0f - mousePos.y;
	std::cout << mousePos << "\n";*/
	if (NCL::Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
		NCL::Maths::Vector2 mousePos = NCL::Window::GetMouse()->GetAbsolutePosition();
		mousePos.y = 720.0f - mousePos.y;

		float rangeX = 1280.0f * scaleX * 0.5f * m_aspect;//x should multiplus aspect
		float rangeY = 720.0f * scaleY * 0.5f;

		float disX = abs(mousePos.x - center.x);
		float disY = abs(mousePos.y - center.y);

		if (disX<=rangeX&&disY<=rangeY)
		{
			return true;
		}
	}
	return false;
}

void DW_UIEventMgr::RegisterUIEvent(const std::string& type, const std::function<void(const std::string&)>& func) {
	m_uiEventDispatcher.appendListener(type, func);
}

void DW_UIEventMgr::TriggerUIEvent(const std::string& type, const std::string& para) {
	m_uiEventDispatcher.dispatch(type,para);
}