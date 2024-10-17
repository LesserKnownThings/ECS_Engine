#pragma once

#include "Assets/Texture.h"
#include "glew/glew.h"
#include "glm/glm.hpp"

namespace LKT
{
    static void UpdateTextureData(const Texture *texture, const glm::vec2 &offset, const glm::vec2 &size)
    {
        float vertices[16] =
            {
                0.5f,
                0.5f,
                1.0f,
                1.0f, // top right
                0.5f,
                -0.5f,
                1.0f,
                0.0f, // bottom right
                -0.5f,
                -0.5f,
                0.0f,
                0.0f, // bottom left
                -0.5f,
                0.5f,
                0.0f,
                1.0f // top left
            };

        uint32_t indices[6] =
            {
                2,
                1,
                0,
                2,
                0,
                3};

        const float xOffset = (offset.x / texture->GetWidth());
        const float yOffset = (offset.y / texture->GetHeight());
        const float width = (size.x / texture->GetWidth());
        const float height = (size.y / texture->GetHeight());

        // bottom left
        vertices[10] = xOffset;
        vertices[11] = yOffset;

        // bottom right
        vertices[6] = xOffset + width;
        vertices[7] = yOffset;

        // top right
        vertices[2] = xOffset + width;
        vertices[3] = yOffset + height;

        // top left
        vertices[14] = xOffset;
        vertices[15] = yOffset + height;

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    }
}