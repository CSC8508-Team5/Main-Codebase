/*
generate a quad for HUD and post effect

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "glad\glad.h"

#include <iostream>

#include "OGLShader.h"

#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"

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

	void BindVAO() { glBindVertexArray(VAO); }
	void Draw() { glDrawArrays(GL_TRIANGLES, 0, 6); glBindVertexArray(0); }


	void Draw(const GLuint tex,const NCL::Maths::Matrix4& v, const NCL::Maths::Matrix4& p, const NCL::Maths::Vector3& pos, const NCL::Maths::Vector3& scale, const NCL::Maths::Matrix4& rot);
private:
	GLuint VAO, VBO;
	NCL::Rendering::OGLShader* m_shader;

	void Init();
};

