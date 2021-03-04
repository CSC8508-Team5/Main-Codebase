#include "DW_UIImageRenderer.h"


void DW_UIImageRenderer::RenderImage(float x, float y, float scaleX, float scaleY, const NCL::Maths::Vector3& color, const unsigned int texture) {
	glUseProgram(m_shader->GetProgramID());

	glUniform3fv(glGetUniformLocation(m_shader->GetProgramID(), "color"), 1, (float*)&color);

	glUniform1i(glGetUniformLocation(m_shader->GetProgramID(), "diffTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	float vertices[] = {x,y,scaleX,scaleY};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, 1);

	glBindVertexArray(0);
}

GLuint DW_UIImageRenderer::LoadTexture(char const* path) {
	return DW_Tools::LoadTexture(path);
}

void DW_UIImageRenderer::Init() {
	m_projection = NCL::Maths::Matrix4::Orthographic(-1.0f, 100.0f, 1280.0f, 0.0f, 720.0f, 0.0f);
	m_shader = new NCL::Rendering::OGLShader("imageVert.glsl", "imageFrag.glsl", "imageGeom.glsl");
	glUseProgram(m_shader->GetProgramID());
	glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgramID(), "projection"), 1, false, (float*)&m_projection);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) *4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}