#pragma once

#include "glm/glm.hpp"

#include <array>

namespace LKT
{
    struct AABB;

    class Camera;

    struct Plane
    {
        Plane() = default;
        Plane(const glm::vec3 &inNormal, const glm::vec3 &inDistance)
            : normal(glm::normalize(inNormal)), d(glm::dot(normal, inDistance))
        {
        }

        glm::vec3 normal;
        float d;
    };

    struct Frustum
    {
        std::array<Plane, 6> faces;
    };

    class FrustumSystem
    {
    public:
        static FrustumSystem &Get();
        static void UpdateFrustum(const Camera *cam);

        static const Frustum &GetFrustum();
        static bool IsInFrustum(const AABB &box);

    private:
        Frustum frustum;
    };
}