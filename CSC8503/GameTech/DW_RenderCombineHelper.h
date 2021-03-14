#pragma once

#include "glad\glad.h"

#include <iostream>

class DW_RenderCombineHelper
{
public:
	DW_RenderCombineHelper(const int w, const int h);
	~DW_RenderCombineHelper();

	GLuint GetFBO() { return m_FBO; }
	GLuint GetTexture() { return m_Tex; }
	GLuint GetBlurTexture() { return m_blurLightTex; }
private:
	GLuint m_FBO,m_Tex,m_RBO;
	GLuint m_blurLightTex;
	int m_width, m_height;
};

