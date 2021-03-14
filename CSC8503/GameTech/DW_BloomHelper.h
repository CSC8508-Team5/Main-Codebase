#pragma once
#include "glad\glad.h"

#include <iostream>

class DW_BloomHelper
{
public:
	DW_BloomHelper(const int w, const int h);
	~DW_BloomHelper();

	GLuint GetPingPangFBO(const int index) { return m_pingpang_fbos[index]; }
	GLuint GetPingPangColorBuffer(const int index) { return m_pingpang_colorBuffer[index]; }
private:
	int m_width, m_height;
	GLuint m_pingpang_fbos[2];
	GLuint m_pingpang_colorBuffer[2];
};

