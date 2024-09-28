#pragma once

#include "glm/glm.hpp"

namespace LKT
{
    struct AABB
    {
        AABB() = default;
        AABB(const glm::vec3 &inMin, const glm::vec3 &inMax)
            : min(inMin), max(inMax) {}

        glm::vec3 min = glm::vec3(0.f);
        glm::vec3 max = glm::vec3(0.f);

        bool Contains(const glm::vec3 &point) const
        {
            return (point.x >= min.x && point.x <= max.x &&
                    point.y >= min.y && point.y <= max.y &&
                    point.z >= min.z && point.z <= max.z);
        }

        bool Intersects(const AABB &other) const
        {
            return (min.x <= other.max.x && max.x >= other.min.x &&
                    min.y <= other.max.y && max.y >= other.min.y &&
                    min.z <= other.max.z && max.z >= other.min.z);
        }

        bool ContainsAABB(const AABB &other) const
        {
            return (other.min.x >= min.x && other.max.x <= max.x &&
                    other.min.y >= min.y && other.max.y <= max.y &&
                    other.min.z >= min.z && other.max.z <= max.z);
        }

        glm::vec3 Size() const
        {
            return glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);
        }

        bool IsInside(const glm::vec4 &plane) const
        {
            // Calculate the distance from the plane to the AABB
            glm::vec3 positiveVertex = (plane.x >= 0) ? glm::vec3(max.x, max.y, max.z) : glm::vec3(min.x, min.y, min.z);
            positiveVertex = (plane.y >= 0) ? positiveVertex : glm::vec3(positiveVertex.x, min.y, positiveVertex.z);
            positiveVertex = (plane.z >= 0) ? positiveVertex : glm::vec3(positiveVertex.x, positiveVertex.y, min.z);

            // Check if the AABB is behind the plane
            if (glm::dot(glm::vec3(plane.x, plane.y, plane.z), positiveVertex) + plane.w < 0)
            {
                // AABB is outside the frustum
                return false;
            }
            // AABB is inside the frustum
            return true;
        }
    };
}