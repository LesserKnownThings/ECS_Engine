#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace LKT
{
    class EngineWindow;
    class Object;

    /// @brief This used to render in a separate buffer, for example a separate window and should only be used in editor
    struct RenderBuffer
    {
        RenderBuffer() = default;
        RenderBuffer(EngineWindow *inParent, std::function<void()> inRenderFunc)
            : parentObj(inParent), renderFunc(inRenderFunc) {}

        EngineWindow *parentObj = nullptr;
        std::function<void()> renderFunc;
        uint32_t fbo, rbo, texture;
        float sizeX, sizeY;
    };    

    class RenderBufferSystem
    {
    public:
        virtual ~RenderBufferSystem();

        static RenderBufferSystem &Get();

        void CreateBuffer(EngineWindow *parent, std::function<void()> inRenderFunc, std::function<void(const EngineWindow *, float, float)> &windowResizeCallback);
        void RemoveBuffer(EngineWindow *window);
        bool GetBufferTexture(EngineWindow *window, uint32_t &outTextureId) const;

        void RenderBuffers();

    private:
        RenderBufferSystem();
        void HandleWindowResized(const EngineWindow *window, float x, float y);

        std::unordered_map<const EngineWindow *, RenderBuffer> buffers;
        std::string renderBufferShaderPath = "RenderBuffer";
    };
}