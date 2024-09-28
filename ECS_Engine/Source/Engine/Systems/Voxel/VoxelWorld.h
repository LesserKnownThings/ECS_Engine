#pragma once

#include "SDL/SDL_events.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

namespace LKT
{
    class Camera;
    class VoxelChunk;
    class VoxelObject;

    class Asset;

    class VoxelWorld
    {
    public:
        ~VoxelWorld();
        VoxelWorld();

    private:
        void PostShaderLoad();
        void ProcessWorld(float deltaTime);
        void RenderWorld();

        void HandleZoom(float amount);
        void HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent);
        void HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent);

        void GatherWorldData();
        void UpdateBuffers(const std::vector<VoxelObject *> &objects);

        Camera *camera = nullptr;
        bool isMouseMoving = false;
        float zoomAmount = 0.0f;

        VoxelChunk *chunk = nullptr;
        LazyAssetPtr<Asset> screenQuad;

        uint32_t octreesBuffer, voxelsBuffer, objectsBuffer;
    };
}