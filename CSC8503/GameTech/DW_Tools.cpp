

#include "DW_Tools.h"


GLuint DW_Tools::LoadTexture(char const* path) {
	stbi_set_flip_vertically_on_load(true);
	bool gammaCorrection = false;
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

float DW_Tools::RandomFloatBetween(float low, float high) {
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - (low))));
}

//calculate speed
NCL::Maths::Vector3 DW_Tools::ComputeVelocity(const NCL::Maths::Vector3& u, const NCL::Maths::Vector3& a, float t) {
	return (u + a * t);
}

//calculate position
NCL::Maths::Vector3 DW_Tools::ComputeEuler(const NCL::Maths::Vector3& s, const NCL::Maths::Vector3& u, const NCL::Maths::Vector3& a, float t) {
	return ComputeVelocity(s, ComputeVelocity(u,a,t),t);
}