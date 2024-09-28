#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace LKT
{
    struct Transform
    {
        void SetPosition(const glm::vec3 &newPos);
        void SetRotation(const glm::vec3 &eulers);

        const glm::vec3 &GetPosition() const { return position; }
        const glm::quat &GetRotation() const { return rotation; }
        const glm::vec3 &GetEulers() const { return glm::eulerAngles(rotation); }

        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 scale = glm::vec3(1.f);
        glm::quat rotation = glm::vec3(0.f);
        glm::mat4 model = glm::mat4(1.f);
    };
}