#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

class DW_Light
{
public:
	enum LightType {
		Directional,
		Point,
		Spot
	};

	DW_Light(const LightType type);
	~DW_Light();

	NCL::Maths::Vector3 GetPosition() { return m_position; }
	void SetPosition(const NCL::Maths::Vector3& pos) { m_position = pos;  }

	NCL::Maths::Vector3 GetDirection() { return m_direction; }
	void SetDirection(const NCL::Maths::Vector3& dir) { m_direction = dir; m_direction.Normalise(); }

	NCL::Maths::Vector4 GetColor() { return m_color; }
	void SetColor(const NCL::Maths::Vector4& color) { m_color = color; }

	float GetAmbient() { return m_ambient; }
	void SetAmbient(const float a) { m_ambient = a; }

	float GetRadius() { return m_radius; }
	void SetRadius(const float r) { m_radius = r; }

private:
	LightType m_type;
	NCL::Maths::Vector3 m_position;//directional light needs a position too, because of the shadow...
	NCL::Maths::Vector3 m_direction;
	NCL::Maths::Vector4 m_color;
	float m_ambient;
	float m_radius;
};

