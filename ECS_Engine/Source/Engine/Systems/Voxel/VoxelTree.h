#pragma once

#include "Entity.h"
#include "glm/glm.hpp"
#include "VoxelPhysics.h"

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>

namespace LKT
{
    struct VoxelModel;
    struct VoxelData;

    constexpr int8_t MAX_DEPTH = 8;
    constexpr int8_t maxVoxelsPerNode = 4;

    struct Voxel
    {
        uint32_t id = 0;
        uint8_t color = 0;
    };

    struct VoxelNode
    {
    public:
        bool isLeaf = false;
        Voxel *voxel;
        VoxelNode *children[8] = {nullptr};
    };

    struct GpuVoxelNode
    {
        int32_t childIndex[8];
        uint32_t voxelIndex;
        uint8_t isLeaf;       
    };

    class VoxelTree
    {
    public:
        VoxelTree();
        VoxelTree(const VoxelModel &data);

        void Insert(const VoxelData &voxel);
        bool Exists(uint32_t x, uint32_t y, uint32_t z) const;

        bool GetGPUData(std::vector<GpuVoxelNode> &nodes, std::vector<Voxel> &voxels);

    private:
        bool GetGPUDataRecursive(VoxelNode *node,
                                 std::vector<GpuVoxelNode> &nodes,
                                 std::vector<Voxel> &voxels);

        void RecursiveInsert(VoxelNode *&node, const VoxelData &voxel, int32_t depth);
        bool ExistsRecursive(VoxelNode *node, uint64_t id, int32_t depth) const;

        VoxelNode *root = nullptr;
        uint8_t maxDepth = MAX_DEPTH;
    };
}