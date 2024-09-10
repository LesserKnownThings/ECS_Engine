#pragma once

#include <cstdint>
#include <string>

namespace LKT
{
    struct GraphicHelpers
    {
        static bool LoadTexture(const std::string &path,
                                uint32_t &textureID,
                                int32_t &width, int32_t &height,
                                uint32_t textureWrapS = 0,
                                uint32_t textureWrapT = 0,
                                uint32_t minFilter = 0,
                                uint32_t maxFilter = 0);

        static void DeleteTexture(uint32_t textureId);

        static bool LoadTexture(const std::string &path, int32_t &width, int32_t &height, int32_t &components, unsigned char *&textureData);

        static bool LoadTexture(int32_t width,
                                int32_t height,
                                int32_t components,
                                void *pixels,
                                uint32_t& outTextureId,
                                uint32_t textureWrapS = 0,
                                uint32_t textureWrapT = 0,
                                uint32_t minFilter = 0,
                                uint32_t maxFilter = 0);
    };
}