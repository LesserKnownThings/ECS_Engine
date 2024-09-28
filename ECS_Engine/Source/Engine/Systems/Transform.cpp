#include "Transform.h"

namespace LKT
{
    void Transform::SetPosition(const glm::vec3 &newPos)
    {
        position = newPos;
        model = glm::translate(glm::mat4(1.f), newPos);
    }

    void Transform::SetRotation(const glm::vec3 &eulers)
    {
        model = glm::mat4(1.f);
        model = glm::rotate(model, glm::radians(eulers.x), glm::vec3(1.f, 0.f, 0.f));
        model = glm::rotate(model, glm::radians(eulers.y), glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, glm::radians(eulers.z), glm::vec3(0.f, 0.f, 1.f));
    }
}