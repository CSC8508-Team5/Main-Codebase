/*
do some tasks related to shadow

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "glad\glad.h"



class DW_ShadowHelper
{
public:
	DW_ShadowHelper();
	~DW_ShadowHelper();

	GLuint GetFBO() { return m_FBO; }
	GLuint GetTexture() { return m_Texture; }

private:
	GLuint SHADOWSIZE = 4096;
	GLuint m_FBO,m_Texture;
};

