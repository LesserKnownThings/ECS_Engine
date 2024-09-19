#include "RenderBufferObject.h"
#include "glew/glew.h"
#include "Systems/OpenGLSystem.h"

#include <iostream>

namespace LKT
{
    RenderBufferObject::~RenderBufferObject()
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);
        glDeleteTextures(1, &texture);
    }

    RenderBufferObject::RenderBufferObject()
        : sizeX(1920), sizeY(1080)
    {
        CreateBuffer();
    }

    RenderBufferObject::RenderBufferObject(int32_t inSizeX, int32_t inSizeY)
        : sizeX(inSizeX), sizeY(inSizeY)
    {
        CreateBuffer();
    }

    void RenderBufferObject::CreateBuffer()
    {
        glGenFramebuffers(1, &fbo);
        glBindBuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(0), static_cast<GLsizei>(0), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // Create renderbuffer object for depth and stencil attachment
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(0), static_cast<GLsizei>(0));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderBufferObject::HandleSizeChanged(int32_t newSizeX, int32_t newSizeY)
    {
        sizeX = newSizeX;
        sizeY = newSizeY;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(sizeX), static_cast<GLsizei>(sizeY), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(sizeX), static_cast<GLsizei>(sizeY));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderBufferObject::Use()
    {
        glViewport(0, 0, sizeX, sizeY);
        glScissor(0, 0, sizeX, sizeY);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderBufferObject::Dispose()
    {
        OpenGLSystem::Get().ResetViewportAndScissor();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}