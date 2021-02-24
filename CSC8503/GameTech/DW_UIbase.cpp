#include "DW_UIbase.h"

DW_UIbase::DW_UIbase(const UIType type, const NCL::Maths::Vector3& pos , const NCL::Maths::Vector3& color , const int priority)
	:m_type{ type }, m_renderPriority{ priority }, m_isEnable{ true }, m_position{ pos }, m_color{ color }, m_name{""}
{
	
}

DW_UIbase::~DW_UIbase() {

}

void DW_UIbase::Update(const float dt) {}

void DW_UIbase::Render() {}