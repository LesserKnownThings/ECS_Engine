#include "Texture.h"
#include "glew.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace LKT
{
	void Texture::LoadAsset(const std::string& inPath)
	{
		__super::LoadAsset(inPath);

		stbi_set_flip_vertically_on_load(false);

		glGenTextures(1, &textureID);

		int32_t components = 0;

		unsigned char* data = stbi_load(path.c_str(), &width, &height, &components, 0);
		if (data != nullptr)
		{
			GLenum format = GL_RED;
			if (components == 1)
				format = GL_RED;
			else if (components == 3)
				format = GL_RGB;
			else if (components == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(data);
		}
	}
}