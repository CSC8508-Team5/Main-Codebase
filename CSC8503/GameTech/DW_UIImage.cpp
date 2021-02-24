#include "DW_UIImage.h"


DW_UIImage::DW_UIImage(const std::string& name, char const* path,const Rect& rect, const NCL::Maths::Vector3& pos, const NCL::Maths::Vector3& color, const int priority)
	:DW_UIbase{ name, UIType::IMAGE,pos,color,priority }, m_rect{ rect }
{
	m_texture = DW_UIImageRenderer::get_instance().LoadTexture(path);
}

DW_UIImage::~DW_UIImage() {
}


void DW_UIImage::Update(const float dt) {
	if (!m_isEnable)
	{
		return;
	}
	if (DW_UIEventMgr::get_instance().GetHasFindTarget())
	{
		return;
	}
	if (DW_UIEventMgr::get_instance().CheckUIImageClick(m_rect.x,m_rect.y,m_position))
	{
		DW_UIEventMgr::get_instance().SetHasFindTarget(true);
		DW_UIEventMgr::get_instance().TriggerUIEvent(m_ParentPanelName,m_name);
	}
}

void DW_UIImage::Render() {
	if (!m_isEnable)
	{
		return;
	}
	DW_UIImageRenderer::get_instance().RenderImage(m_position.x, m_position.y, m_rect.x,m_rect.y,m_color,m_texture);
}