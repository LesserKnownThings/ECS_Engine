#pragma once

#include "Object.h"
#include "Systems/Transform.h"
#include "VoxelPhysics.h"

namespace LKT
{
    struct VoxelModel;

    class ChunkNode;
    class VoxelTree;

    class VoxelObject : public Object
    {
    public:
        ~VoxelObject();
        VoxelObject();

        void CreateVoxelTree(const VoxelModel &model);
        VoxelTree *GetCollisionTree() const { return collisionTree; }

        const Transform &GetTransform() const { return transform; }
        Transform &GetTransform() { return transform; }

        // Returns the world collision box, not the local one
        const AABB GetCollisionBox() const;

    private:
        void PhysicsProcess(float deltaTime);
        void Process(float deltaTime);

        ChunkNode *nodeRef = nullptr;
        VoxelTree *collisionTree = nullptr;
        Transform transform;
        AABB collisionBox;
    };
}