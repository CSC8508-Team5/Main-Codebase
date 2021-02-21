#pragma once

#include "glad\glad.h"

#include <map>
#include <string>

#include "../../Plugins/FreeType/ft2build.h"
#include FT_FREETYPE_H

#include "OGLShader.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"

#include "DW_UIbase.h"




class DW_UIText :public DW_UIbase
{
public:
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		NCL::Maths::Vector2   Size;      // Size of glyph
		NCL::Maths::Vector2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	DW_UIText();
	virtual ~DW_UIText();

	virtual void Update(const float dt);

	virtual void Render();

	void RenderText(std::string text, float x, float y, float scale, NCL::Maths::Vector3 color);
private:
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
	NCL::Rendering::OGLShader* m_shader;
	NCL::Maths::Matrix4 m_projection;
	FT_Library ft;
	FT_Face face;
};

