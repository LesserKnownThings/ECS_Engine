#include "VoxelChunk.h"
#include "VoxelObject.h"
#include "VoxelTree.h"
#include "Systems/FrustumSystem.h"

namespace LKT
{
    VoxelChunk::VoxelChunk()
    {
        root = new ChunkNode(AABB(glm::vec3(0.f), glm::vec3(1.f)), 0);
    }

    VoxelChunk::VoxelChunk(const glm::vec3 &size)
    {
        root = new ChunkNode(AABB(glm::vec3(0.f), size), 0);
    }

    void VoxelChunk::Insert(VoxelObject *tree)
    {
        root->Insert(tree);
    }

    bool VoxelChunk::Query(const glm::vec3 &point, std::vector<VoxelObject *> &outObjects) const
    {
        return root->Query(point, outObjects);
    }

    bool VoxelChunk::QueryWithFrustum(std::vector<VoxelObject *> &outObjects) const
    {
        return root->QueryWithFrustum(outObjects);
    }

    bool ChunkNode::Query(const glm::vec3 &point, std::vector<VoxelObject *> &outObjects) const
    {
        if (!boundary.Contains(point))
        {
            return false;
        }

        if (subdivided)
        {
            for (int32_t i = 0; i < 8; ++i)
            {
                if (children[i]->Query(point, outObjects))
                {
                    return true;
                }
            }
        }

        outObjects = objects;
        return true;
    }

    bool ChunkNode::QueryWithFrustum(std::vector<VoxelObject *> &outObjects) const
    {
        if (!FrustumSystem::IsInFrustum(boundary))
        {
            return false;
        }

        for (VoxelObject *tree : objects)
        {
            outObjects.push_back(tree);
        }

        if (subdivided)
        {
            for (ChunkNode *child : children)
            {
                child->QueryWithFrustum(outObjects);
            }
        }

        return true;
    }

    bool ChunkNode::Insert(VoxelObject *object)
    {
        const AABB collisionBox = object->GetCollisionBox();
        if (!boundary.Intersects(collisionBox))
        {
            return false;
        }

        if (depth == MAX_DEPTH || !CanSubdivide(collisionBox))
        {
            objects.push_back(object);
            return true;
        }

        if (!subdivided)
        {
            Subdivide();
        }

        for (int32_t i = 0; i < 8; ++i)
        {
            if (children[i]->Insert(object))
            {
                return true;
            }
        }

        objects.push_back(object);
        return true;
    }

    bool ChunkNode::CanSubdivide(const AABB &box)
    {
        const glm::vec3 size = boundary.Size();
        const glm::vec3 otherSize = box.Size();
        return (otherSize.x < size.x / 2.0f &&
                otherSize.y < size.y / 2.0f &&
                otherSize.z < size.z / 2.0f);
    }

    void ChunkNode::Subdivide()
    {
        subdivided = true;
        glm::vec3 mid = glm::vec3((boundary.min.x + boundary.max.x) / 2.0f,
                                  (boundary.min.y + boundary.max.y) / 2.0f,
                                  (boundary.min.z + boundary.max.z) / 2.0f);

        glm::vec3 size = boundary.Size() / 2.0f;
        const int32_t newDepth = depth + 1;

        children[0] = new ChunkNode(AABB{boundary.min, mid}, newDepth); // Bottom-left-front
        children[1] = new ChunkNode(AABB(glm::vec3(mid.x, boundary.min.y, boundary.min.z),
                                         glm::vec3(boundary.max.x, mid.y, mid.z)),
                                    newDepth); // Bottom-right-front
        children[2] = new ChunkNode(AABB(glm::vec3(boundary.min.x, mid.y, boundary.min.z),
                                         glm::vec3(mid.x, boundary.max.y, mid.z)),
                                    newDepth); // Top-left-front
        children[3] = new ChunkNode(AABB(glm::vec3(mid.x, mid.y, boundary.min.z), boundary.max),
                                    newDepth); // Top-right-front

        // Repeat for the back four
        children[4] = new ChunkNode(AABB(glm::vec3(boundary.min.x, boundary.min.y, mid.z),
                                         glm::vec3(mid.x, mid.y, boundary.max.z)),
                                    newDepth); // Bottom-left-back
        children[5] = new ChunkNode(AABB(glm::vec3(mid.x, boundary.min.y, mid.z),
                                         glm::vec3(boundary.max.x, mid.y, boundary.max.z)),
                                    newDepth); // Bottom-right-back
        children[6] = new ChunkNode(AABB(glm::vec3(boundary.min.x, mid.y, mid.z),
                                         glm::vec3(mid.x, boundary.max.y, boundary.max.z)),
                                    newDepth); // Top-left-back
        children[7] = new ChunkNode(AABB(mid, boundary.max), newDepth);
    }
}