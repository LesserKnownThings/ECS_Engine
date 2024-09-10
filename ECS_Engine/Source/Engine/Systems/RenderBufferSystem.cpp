#include "RenderBufferSystem.h"
#include "glew/glew.h"
#include "glm/glm.hpp"
#include "Object.h"
#include "OpenGLSystem.h"
#include "ShaderSystem/ShaderManager.h"
#include "TaskManagerSystem.h"
#include "UI/EngineWindow.h"

namespace LKT
{
    RenderBufferSystem::~RenderBufferSystem()
    {
        TaskManagerSystem::Get().RemoveAllTasks(this);
    }

    RenderBufferSystem &RenderBufferSystem::Get()
    {
        static RenderBufferSystem instance;
        return instance;
    }

    void RenderBufferSystem::CreateBuffer(EngineWindow *parent,
                                          std::function<void()> inRenderFunc,
                                          std::function<void(const EngineWindow *, float, float)> &windowResizeCallback)
    {
        RenderBuffer renderBuffer{parent, inRenderFunc};

        glGenFramebuffers(1, &renderBuffer.fbo);
        glBindBuffer(GL_FRAMEBUFFER, renderBuffer.fbo);

        glGenTextures(1, &renderBuffer.texture);
        glBindTexture(GL_TEXTURE_2D, renderBuffer.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(0), static_cast<GLsizei>(0), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderBuffer.texture, 0);

        // Create renderbuffer object for depth and stencil attachment
        glGenRenderbuffers(1, &renderBuffer.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer.rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(0), static_cast<GLsizei>(0));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        windowResizeCallback = std::bind(&RenderBufferSystem::HandleWindowResized, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        buffers.emplace(parent, renderBuffer);
    }

    void RenderBufferSystem::RemoveBuffer(EngineWindow *window)
    {
        const auto it = buffers.find(window);
        if (it != buffers.end())
        {
            it->second.parentObj = nullptr;
        }
    }

    bool RenderBufferSystem::GetBufferTexture(EngineWindow *window, uint32_t &outTextureId) const
    {
        const auto it = buffers.find(window);
        if (it != buffers.end())
        {
            outTextureId = it->second.texture;
            return true;
        }
        return false;
    }

    void RenderBufferSystem::RenderBuffers()
    {
        for (auto it = buffers.begin(); it != buffers.end();)
        {
            if (it->second.parentObj != nullptr)
            {
                glViewport(0, 0, it->second.sizeX, it->second.sizeY);
                glScissor(0, 0, it->second.sizeX, it->second.sizeY);

                glBindFramebuffer(GL_FRAMEBUFFER, it->second.fbo);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                it->second.renderFunc();
                ++it;
            }
            else
            {
                it = buffers.erase(it);
            }
        }

        OpenGLSystem::Get().ResetViewport();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    RenderBufferSystem::RenderBufferSystem()
    {
        TaskManagerSystem::Get().RegisterTask(this, &RenderBufferSystem::RenderBuffers, 0, RENDER_HANDLE);
    }

    void RenderBufferSystem::HandleWindowResized(const EngineWindow *window, float x, float y)
    {
        const auto it = buffers.find(window);

        if (it != buffers.end())
        {
            RenderBuffer &rb = it->second;

            rb.sizeX = x;
            rb.sizeY = y;

            glBindFramebuffer(GL_FRAMEBUFFER, rb.fbo);

            glBindTexture(GL_TEXTURE_2D, rb.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(x), static_cast<GLsizei>(y), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rb.texture, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, rb.rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(x), static_cast<GLsizei>(y));
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb.rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "Framebuffer is not complete!" << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}