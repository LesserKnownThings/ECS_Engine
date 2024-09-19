#pragma once

#include <cstdint>

namespace LKT
{
    class RenderBufferObject
    {
    public:
        ~RenderBufferObject();
        RenderBufferObject();
        RenderBufferObject(int32_t inSizeX, int32_t inSizeY);

        void HandleSizeChanged(int32_t newSizeX, int32_t newSizeY);

        void Use();
        void Dispose();

        uint32_t GetTextureId() const { return texture; }

    private:
        void CreateBuffer();

        uint32_t fbo, rbo, texture;
        int32_t sizeX;
        int32_t sizeY;
    };
}