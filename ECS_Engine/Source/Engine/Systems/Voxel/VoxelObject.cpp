#include "VoxelObject.h"
#include "Systems/TaskManagerSystem.h"
#include "VoxelModel.h"
#include "VoxelTree.h"

namespace LKT
{
    VoxelObject::~VoxelObject()
    {
    }

    VoxelObject::VoxelObject()
    {
    }

    void VoxelObject::CreateVoxelTree(const VoxelModel &model)
    {
        collisionBox.min = glm::vec3(0.f);
        collisionBox.max = glm::vec3((float)model.size.x,
                                     (float)model.size.y,
                                     (float)model.size.z);
        collisionTree = new VoxelTree(model);
    }

    const AABB VoxelObject::GetCollisionBox() const
    {
        AABB worldCollisionBox = collisionBox;
        worldCollisionBox.min += transform.position;
        worldCollisionBox.max += transform.position;
        return worldCollisionBox;
    }

    void VoxelObject::PhysicsProcess(float deltaTime)
    {
    }

    void VoxelObject::Process(float deltaTime)
    {
    }
}