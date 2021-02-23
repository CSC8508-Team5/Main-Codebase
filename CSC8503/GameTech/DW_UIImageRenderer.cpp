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
	stbi_set_flip_vertically_on_load(true);
	bool gammaCorrection = false;
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void DW_UIImageRenderer::Init() {
	m_projection = NCL::Maths::Matrix4::Orthographic(-1.0f, 1.0f, 1280.0f, 0.0f, 720.0f, 0.0f);
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