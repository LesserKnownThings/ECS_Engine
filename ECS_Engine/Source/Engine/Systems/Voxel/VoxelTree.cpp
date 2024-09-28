#include "VoxelTree.h"
#include "VoxelModel.h"

#include <cstring>
#include <iostream>

namespace LKT
{
    namespace
    {
        // Morton code
        static uint32_t SpreadBits(uint32_t x)
        {
            x = (x | (x << 16)) & 0x1F0000FF0000FF;
            x = (x | (x << 8)) & 0x100F00F00F00F00F;
            x = (x | (x << 4)) & 0x10C30C30C30C30C3;
            x = (x | (x << 2)) & 0x1249249249249249;
            return x;
        }

        static uint64_t MortonEncode(uint32_t x, uint32_t y, uint32_t z)
        {
            return (SpreadBits(x) | (SpreadBits(y) << 1) | (SpreadBits(z) << 2));
        }
    }

    VoxelTree::VoxelTree()
    {
    }

    VoxelTree::VoxelTree(const VoxelModel &data)
        : root(new VoxelNode())
    {
        const int32_t voxels = data.size.x * data.size.y * data.size.z;
        const float logValue = std::log2(static_cast<float>(voxels));
        maxDepth = static_cast<int32_t>(std::ceil(logValue / 3.0f));

        for (int32_t i = 0; i < data.nVoxels; ++i)
        {
            const VoxelData &voxel = data.voxels[i];
            Insert(voxel);
        }
    }

    void VoxelTree::Insert(const VoxelData &voxel)
    {
        RecursiveInsert(root, voxel, 0);
    }

    bool VoxelTree::GetGPUData(std::vector<GpuVoxelNode> &nodes, std::vector<Voxel> &voxels)
    {
        return GetGPUDataRecursive(root, nodes, voxels);
    }

    bool VoxelTree::GetGPUDataRecursive(VoxelNode *node, std::vector<GpuVoxelNode> &nodes, std::vector<Voxel> &voxels)
    {
        GpuVoxelNode gpuNode;

        if (node->isLeaf)
        {
            gpuNode.isLeaf = true;

            Voxel &vox = *node->voxel;
            gpuNode.voxelIndex = voxels.size();
            voxels.push_back(vox);

            for (int32_t i = 0; i < 8; ++i)
            {
                gpuNode.childIndex[i] = -1;
            }
        }
        else
        {
            gpuNode.isLeaf = 0;
            for (int i = 0; i < 8; ++i)
            {
                if (node->children[i])
                {
                    gpuNode.childIndex[i] = nodes.size() + 1 + i; // Index of the next child in the array
                    GetGPUDataRecursive(node->children[i], nodes, voxels);
                }
                else
                {
                    gpuNode.childIndex[i] = -1;
                }
            }
            gpuNode.voxelIndex = -1;
        }

        nodes.push_back(gpuNode);
        return true;
    }

    void VoxelTree::RecursiveInsert(VoxelNode *&node, const VoxelData &voxel, int32_t depth)
    {
        const uint64_t voxelId = LKT::MortonEncode(voxel.x, voxel.y, voxel.z);

        if (depth == maxDepth)
        {
            if (node == nullptr)
            {
                node = new VoxelNode();
            }

            Voxel *newVox = new Voxel();
            newVox->color = voxel.colorIndex;
            newVox->id = voxelId;
            node->voxel = newVox;
            node->isLeaf = true;
            return;
        }

        if (node == nullptr)
        {
            node = new VoxelNode();
        }

        int childIndex = (voxelId >> (3 * (maxDepth - depth - 1))) & 0x7;
        RecursiveInsert(node->children[childIndex], voxel, depth + 1);
    }

    bool VoxelTree::Exists(uint32_t x, uint32_t y, uint32_t z) const
    {
        const uint64_t id = LKT::MortonEncode(x, y, z);
        return ExistsRecursive(root, id, 0);
    }

    bool VoxelTree::ExistsRecursive(VoxelNode *node, uint64_t id, int32_t depth) const
    {
        if (node == nullptr)
        {
            return false;
        }

        if (depth == maxDepth)
        {
            return node->voxel != nullptr;
        }

        int childIndex = (id >> (3 * (maxDepth - depth - 1))) & 0x7;
        return ExistsRecursive(node->children[childIndex], id, depth + 1);
    }
}