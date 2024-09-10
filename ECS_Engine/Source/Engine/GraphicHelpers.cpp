#include "GraphicHelpers.h"

#include "glew/glew.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image/stb_image.h"

namespace LKT
{
    bool GraphicHelpers::LoadTexture(const std::string &path,
                                     uint32_t &textureID,
                                     int32_t &width,
                                     int32_t &height,
                                     uint32_t textureWrapS,
                                     uint32_t textureWrapT,
                                     uint32_t minFilter,
                                     uint32_t maxFilter)
    {
        stbi_set_flip_vertically_on_load(false);

        glGenTextures(1, &textureID);

        int32_t components = 0;

        unsigned char *data = stbi_load(path.c_str(), &width, &height, &components, 0);
        if (data != nullptr)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapS == 0 ? GL_REPEAT : textureWrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapT == 0 ? GL_REPEAT : textureWrapT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter == 0 ? GL_LINEAR_MIPMAP_LINEAR : minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter == 0 ? GL_LINEAR : maxFilter);

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

            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);

            return true;
        }

        return false;
    }

    void GraphicHelpers::DeleteTexture(uint32_t textureId)
    {
        glDeleteTextures(1, &textureId);
    }

    bool GraphicHelpers::LoadTexture(const std::string &path, int32_t &width, int32_t &height, int32_t &components, unsigned char *&textureData)
    {
        textureData = stbi_load(path.c_str(), &width, &height, &components, 0);
        return textureData != nullptr;
    }

    bool GraphicHelpers::LoadTexture(int32_t width,
                                     int32_t height,
                                     int32_t components,
                                     void *pixels,
                                     uint32_t& outTextureId,
                                     uint32_t textureWrapS,
                                     uint32_t textureWrapT,
                                     uint32_t minFilter,
                                     uint32_t maxFilter)
    {
        glGenTextures(1, &outTextureId);

        if (pixels != nullptr)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapS == 0 ? GL_REPEAT : textureWrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapT == 0 ? GL_REPEAT : textureWrapT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter == 0 ? GL_LINEAR_MIPMAP_LINEAR : minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter == 0 ? GL_LINEAR : maxFilter);

            GLenum format = GL_RED;
            if (components == 1)
                format = GL_RED;
            else if (components == 3)
                format = GL_RGB;
            else if (components == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, outTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            return true;
        }

        return false;
    }
}