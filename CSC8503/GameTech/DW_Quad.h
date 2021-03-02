/*
generate a quad for HUD and post effect

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "glad\glad.h"

#include <iostream>

#include "DW_Singleton.h"

class DW_Quad:public DW_Singleton<DW_Quad>
{
public:
	DW_Quad(token) {
		std::cout << "DW_Quad Init!\n";
		Init();
	}

	~DW_Quad() {
		std::cout << "DW_Quad!\n";
		
	}

	DW_Quad(const DW_Quad&) = delete;
	DW_Quad& operator =(const DW_Quad&) = delete;

private:
	GLuint VAO, VBO;

	void Init();
};

