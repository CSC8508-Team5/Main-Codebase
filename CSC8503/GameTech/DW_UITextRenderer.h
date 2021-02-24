/*
Part of UI System code.

renderer of ui text component

Comments and queries to: Dawei Wang, Group 5
*/


#pragma once

#include "glad\glad.h"

#include <string>
#include <iostream>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "OGLShader.h"
#include "../../Common/Assets.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"



#include "DW_Singleton.h"

class DW_UITextRenderer:public DW_Singleton<DW_UITextRenderer>
{
public:
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		NCL::Maths::Vector2   Size;      // Size of glyph
		NCL::Maths::Vector2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	DW_UITextRenderer(token) {
		std::cout << "DW_UITextRenderer Init!\n";
		Init();
	}

	~DW_UITextRenderer() {
		std::cout << "DW_UITextRenderer delete!\n";
		delete m_shader;
	}

	DW_UITextRenderer(const DW_UITextRenderer&) = delete;
	DW_UITextRenderer& operator =(const DW_UITextRenderer&) = delete;

	void RenderText(std::string text, float x, float y, float scale, NCL::Maths::Vector3 color);

	

protected:
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
	NCL::Rendering::OGLShader* m_shader;
	NCL::Maths::Matrix4 m_projection;
	FT_Library ft;
	FT_Face face;

	void Init();
};

