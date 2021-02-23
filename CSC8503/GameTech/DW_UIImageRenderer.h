#pragma once

#include "glad\glad.h"

#include <iostream>
#include <string>

#include "OGLShader.h"

#include "../../Common/Assets.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"

#include "../../Common/stb/stb_image.h"

#include "../../Common/Assets.h"

#include "DW_Singleton.h"


class DW_UIImageRenderer:public DW_Singleton<DW_UIImageRenderer>
{
public:
	DW_UIImageRenderer(token) {
		std::cout << "DW_UIImageRenderer Init!\n";
		Init();
	}

	~DW_UIImageRenderer() {
		std::cout << "DW_UIImageRenderer delete!\n";
		delete m_shader;
	}

	DW_UIImageRenderer(const DW_UIImageRenderer&) = delete;
	DW_UIImageRenderer& operator =(const DW_UIImageRenderer&) = delete;

	void RenderImage(float x, float y, float scaleX, float scaleY, const NCL::Maths::Vector3& color, const unsigned int texture);

	GLuint LoadTexture(char const* path );
private:
	GLuint VAO, VBO;
	NCL::Rendering::OGLShader* m_shader;
	NCL::Maths::Matrix4 m_projection;

	void Init();
};

