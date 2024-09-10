#include "Mesh.h"
#include "Systems/MeshLoadingSystem.h"
#include "Systems/OpenGLSystem.h"

namespace LKT
{
    void Mesh::UnloadAsset()
    {
        OpenGLSystem::UnloadRenderComponentObject(rco);
    }

    const RenderComponentObject &Mesh::GetRenderComponentObject() const
    {
        return rco;
    }

    void Mesh::LoadAsset(void *buffer)
    {
        MeshData meshData;
        meshData.ReadBuffer(buffer);
        OpenGLSystem::GetRenderComponentObject(meshData, rco);
    }
}
