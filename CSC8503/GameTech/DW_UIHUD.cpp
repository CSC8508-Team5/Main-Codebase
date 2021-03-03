#include "DW_UIHUD.h"
#include "DW_Tools.h"

DW_UIHUD::DW_UIHUD(char const* path, const NCL::Maths::Vector2& scale, const NCL::Maths::Vector3& offset)
	:m_scale{ scale }, m_offset{offset}
{
	m_texture = DW_Tools::LoadTexture(path);
}


DW_UIHUD::~DW_UIHUD() {


}
