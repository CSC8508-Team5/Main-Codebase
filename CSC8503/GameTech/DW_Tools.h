#pragma once

#include "glad\glad.h"
#include "../../Common/stb/stb_image.h"
#include <iostream>
#include <string>

class DW_Tools
{
public:
	static GLuint LoadTexture(char const* path);
};

