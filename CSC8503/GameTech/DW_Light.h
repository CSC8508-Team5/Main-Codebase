/*
store light data

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once



#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

#include "../../Plugins/OpenGLRendering/OGLMesh.h"

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

	int GetType() { return (int)m_type; }

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

	NCL::Rendering::OGLMesh* GetLightingVolume() { return m_lightingVolume; }
	void SetLightingVolume(NCL::Rendering::OGLMesh* m) { m_lightingVolume = m; }

	float GetCutoff() { return m_cutoff; }
	void SetCutoff(const float c) { m_cutoff = c; }
private:
	LightType m_type;
	NCL::Maths::Vector3 m_position;//directional light needs a position too, because of the shadow...
	NCL::Maths::Vector3 m_direction;
	NCL::Maths::Vector4 m_color;
	float m_ambient;
	float m_radius;
	float m_cutoff;
	NCL::Rendering::OGLMesh* m_lightingVolume = nullptr;
};

