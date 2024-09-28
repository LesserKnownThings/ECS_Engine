#include "FrustumSystem.h"
#include "Camera.h"
#include "Voxel/VoxelPhysics.h"

namespace LKT
{
    FrustumSystem &FrustumSystem::Get()
    {
        static FrustumSystem instance;
        return instance;
    }

    void FrustumSystem::UpdateFrustum(const Camera *cam)
    {
        FrustumSystem &instance = Get();
        Frustum &frustum = instance.frustum;

        std::array<Plane, 6> &planes = frustum.faces;
        glm::mat4 clipMatrix = cam->GetProjection() * cam->GetView();

        // Left plane
        planes[0].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][0],
                                     clipMatrix[1][3] + clipMatrix[1][0],
                                     clipMatrix[2][3] + clipMatrix[2][0]);
        planes[0].d = clipMatrix[3][3] + clipMatrix[3][0];
        planes[0].normal = glm::normalize(planes[0].normal);

        // Right plane
        planes[1].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][0],
                                     clipMatrix[1][3] - clipMatrix[1][0],
                                     clipMatrix[2][3] - clipMatrix[2][0]);
        planes[1].d = clipMatrix[3][3] - clipMatrix[3][0];
        planes[1].normal = glm::normalize(planes[1].normal);

        // Bottom plane
        planes[2].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][1],
                                     clipMatrix[1][3] + clipMatrix[1][1],
                                     clipMatrix[2][3] + clipMatrix[2][1]);
        planes[2].d = clipMatrix[3][3] + clipMatrix[3][1];
        planes[2].normal = glm::normalize(planes[2].normal);

        // Top plane
        planes[3].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][1],
                                     clipMatrix[1][3] - clipMatrix[1][1],
                                     clipMatrix[2][3] - clipMatrix[2][1]);
        planes[3].d = clipMatrix[3][3] - clipMatrix[3][1];
        planes[3].normal = glm::normalize(planes[3].normal);

        // Near plane
        planes[4].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][2],
                                     clipMatrix[1][3] + clipMatrix[1][2],
                                     clipMatrix[2][3] + clipMatrix[2][2]);
        planes[4].d = clipMatrix[3][3] + clipMatrix[3][2];
        planes[4].normal = glm::normalize(planes[4].normal);

        // Far plane
        planes[5].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][2],
                                     clipMatrix[1][3] - clipMatrix[1][2],
                                     clipMatrix[2][3] - clipMatrix[2][2]);
        planes[5].d = clipMatrix[3][3] - clipMatrix[3][2];
        planes[5].normal = glm::normalize(planes[5].normal);
    }

    const Frustum &FrustumSystem::GetFrustum()
    {
        return Get().frustum;
    }

    bool FrustumSystem::IsInFrustum(const AABB &box)
    {
        const Frustum &frustum = GetFrustum();

        for (int32_t i = 0; i < 6; ++i)
        {
            const glm::vec4 frustumVec(frustum.faces[i].normal, frustum.faces[i].d);
            if (box.IsInside(frustumVec))
            {
                return true;
            }
        }
        return false;
    }
}