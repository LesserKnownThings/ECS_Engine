#pragma once

#include "Entity.h"
#include "VoxelPhysics.h"

#include <unordered_map>

namespace LKT
{
    class VoxelObject;

    constexpr int32_t MAX_OBJECT_PER_SUBDIVISION = 8;

    struct FlatVoxelNode
    {
        uint32_t flags;
        uint32_t childIndex;
        uint8_t voxelData;
    };

    struct ChunkNode
    {
        ChunkNode(AABB inBoundary, int32_t inDepth)
            : boundary(inBoundary), depth(inDepth) {}

        int32_t depth = 0;
        bool subdivided = false;
        AABB boundary;

        std::vector<VoxelObject *> objects;

        ChunkNode *children[8];

        bool Query(const glm::vec3 &point, std::vector<VoxelObject *> &outObjects) const;
        bool QueryWithFrustum(std::vector<VoxelObject *> &outObjects) const;
        bool Insert(VoxelObject *obj);
        bool CanSubdivide(const AABB &box);
        void Subdivide();
    };

    class VoxelChunk
    {
    public:
        VoxelChunk();
        VoxelChunk(const glm::vec3 &size);

        void Insert(VoxelObject *tree);
        bool Query(const glm::vec3 &point, std::vector<VoxelObject *> &outObjects) const;
        bool QueryWithFrustum(std::vector<VoxelObject *> &outObjects) const;

    private:
        ChunkNode *root = nullptr;
    };
}