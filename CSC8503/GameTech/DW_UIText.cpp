#include "DW_UIText.h"

DW_UIText::DW_UIText(const std::string& name, const std::string& str, const float scale , const NCL::Maths::Vector3& pos, const NCL::Maths::Vector3& color, const int priority)
	:DW_UIbase{ name, UIType::TEXT,pos,color,priority }, m_text{ str }, m_scale{scale} {
	
}


DW_UIText::~DW_UIText() {

}

void DW_UIText::Update(const float dt) {
	if (!m_isEnable)
	{
		return;
	}
}

void DW_UIText::Render() {
	if (!m_isEnable)
	{
		return;
	}
	DW_UITextRenderer::get_instance().RenderText(m_text, m_position.x, m_position.y, m_scale, m_color);
}

