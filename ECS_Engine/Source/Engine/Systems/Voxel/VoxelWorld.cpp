#include "VoxelWorld.h"
#include "Assets/Mesh.h"
#include "Camera.h"
#include "glew/glew.h"
#include "glm/glm.hpp"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/FrustumSystem.h"
#include "Systems/TaskManagerSystem.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "VoxelChunk.h"
#include "VoxelModel.h"
#include "Voximport.h"
#include "VoxelTree.h"
#include "VoxelObject.h"

namespace LKT
{
    namespace
    {
        void getAABBVertices(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float *vertices)
        {
            // Define the 8 vertices of the AABB (cuboid)
            vertices[0] = minX;
            vertices[1] = minY;
            vertices[2] = minZ; // 0: Bottom-front-left
            vertices[3] = maxX;
            vertices[4] = minY;
            vertices[5] = minZ; // 1: Bottom-front-right
            vertices[6] = maxX;
            vertices[7] = maxY;
            vertices[8] = minZ; // 2: Top-front-right
            vertices[9] = minX;
            vertices[10] = maxY;
            vertices[11] = minZ; // 3: Top-front-left

            vertices[12] = minX;
            vertices[13] = minY;
            vertices[14] = maxZ; // 4: Bottom-back-left
            vertices[15] = maxX;
            vertices[16] = minY;
            vertices[17] = maxZ; // 5: Bottom-back-right
            vertices[18] = maxX;
            vertices[19] = maxY;
            vertices[20] = maxZ; // 6: Top-back-right
            vertices[21] = minX;
            vertices[22] = maxY;
            vertices[23] = maxZ; // 7: Top-back-left
        }

        void getAABBIndices(unsigned int *indices)
        {
            // Define the 12 edges of the AABB (lines connecting the vertices)
            unsigned int lineIndices[] = {
                0, 1, 1, 2, 2, 3, 3, 0, // Front face
                4, 5, 5, 6, 6, 7, 7, 4, // Back face
                0, 4, 1, 5, 2, 6, 3, 7  // Connecting front to back
            };
            for (int i = 0; i < 24; ++i)
            {
                indices[i] = lineIndices[i];
            }
        }
    }

    VoxelWorld::~VoxelWorld()
    {
        TaskManagerSystem::Get().RemoveAllTasks(this);
    }

    VoxelWorld::VoxelWorld()
    {
        ShaderManager::Get().onPostCompile.Bind(this, &VoxelWorld::PostShaderLoad);

        TaskManagerSystem::Get().RegisterTask(this, &VoxelWorld::RenderWorld, 0, RENDER_HANDLE);
        TaskManagerSystem::Get().RegisterTask(this, &VoxelWorld::ProcessWorld, 0, PROCESS_HANDLE);

        InputManagerSystem::Get().onMouseScroll.Bind(this, &VoxelWorld::HandleZoom);
        InputManagerSystem::Get().onMousePressed.Bind(this, &VoxelWorld::HandleMousePressed);
        InputManagerSystem::Get().onMouseReleased.Bind(this, &VoxelWorld::HandleMouseReleased);

        /**
         * So the world will be able to have 10k of the biggest entity which is 256 so it's pretty big
         * That being said, I'm not sure this will work, I will probably have to subdivide the world
         * in more chunks.
         */
        chunk = new VoxelChunk(glm::vec3(256));

        VoxelModel model;
        Voximport::Import("Data/test.vox", model);

        VoxelObject *obj = new VoxelObject();
        obj->GetTransform().SetPosition(glm::vec3(0, 0, 0));
        obj->CreateVoxelTree(model);

        VoxelObject *obj2 = new VoxelObject();
        obj2->GetTransform().SetPosition(glm::vec3(100, 0, 0));
        obj2->CreateVoxelTree(model);

        chunk->Insert(obj);
        //chunk->Insert(obj2);
        // chunk->Insert(obj3);

        auto func = [this](const LazyAssetPtr<Asset> &asset)
        {
            screenQuad = std::move(asset.StrongRef());
        };

        AssetManager::GetAsset(AssetPath{"Data/Content/Assets/Plane.asset"}, func);
    }

    void VoxelWorld::PostShaderLoad()
    {
        camera = new Camera(1920, 1080);
        FrustumSystem::UpdateFrustum(camera);
    }

    void VoxelWorld::ProcessWorld(float deltaTime)
    {
        FrustumSystem::UpdateFrustum(camera);

        if (camera != nullptr)
        {
            camera->Zoom(zoomAmount * deltaTime * 100.0f);

            glm::vec2 mouseDelta = InputManagerSystem::Get().GetMouseDelta();

            glm::vec3 movementVec = InputManagerSystem::Get().GetHorizontalAxis() *
                                        camera->GetRightVector() +
                                    InputManagerSystem::Get().GetVerticalAxis() * camera->GetForwardVector();

            if (isMouseMoving)
            {
                mouseDelta *= deltaTime * 10.f;
                camera->Rotate(glm::vec3(-mouseDelta.y, -mouseDelta.x, 0.0f));

                camera->Move(movementVec * deltaTime * 100.f);
            }
        }
    }

    void VoxelWorld::RenderWorld()
    {
        if (camera)
        {
            ShaderManager::Get().ActivateShader("Voxel_Lit");
            ShaderManager::Get().SetVec3("cameraPos", camera->GetPosition());

            if (const Mesh *mesh = screenQuad.Get<Mesh>())
            {
                camera->Present();
                const RenderComponentObject &rco = mesh->GetRenderComponentObject();
                glBindVertexArray(rco.vao);
                glDrawElements(GL_TRIANGLES, rco.elementCount, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                GatherWorldData();
            }
        }
    }

    void VoxelWorld::HandleZoom(float amount)
    {
        zoomAmount = amount;
    }

    void VoxelWorld::HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            isMouseMoving = true;
        }
    }

    void VoxelWorld::GatherWorldData()
    {
        std::vector<VoxelObject *> visibleObjects;
        chunk->QueryWithFrustum(visibleObjects);
        std::cout << visibleObjects.size() << std::endl;

        ShaderManager::Get().SetInt("visibleObjects", visibleObjects.size());

        static bool test = false;
        if (!test)
        {
            test = true;
            UpdateBuffers(visibleObjects);
        }
    }

    void VoxelWorld::UpdateBuffers(const std::vector<VoxelObject *> &objects)
    {
        struct alignas(16) Obj
        {
            glm::mat4 model;
            alignas(16) glm::vec3 min;
            alignas(16) glm::vec3 max;
        };

        glGenBuffers(1, &objectsBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectsBuffer);

        char *buffer = reinterpret_cast<char *>(malloc(sizeof(Obj) * objects.size()));
        for (int32_t i = 0; i < objects.size(); ++i)
        {
            const AABB &aabb = objects[i]->GetCollisionBox();
            Obj newObj{objects[i]->GetTransform().model, aabb.min, aabb.max};
            memcpy(buffer + i * sizeof(Obj), &newObj, sizeof(Obj));
        }

        const uint32_t bufferSize = sizeof(Obj);

        glBufferData(GL_SHADER_STORAGE_BUFFER, objects.size() * sizeof(Obj), buffer, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, objectsBuffer);

        std::vector<GpuVoxelNode> nodes;
        std::vector<Voxel> voxels;

        for (int32_t i = 0; i < objects.size(); ++i)
        {
            objects[i]->GetCollisionTree()->GetGPUData(nodes, voxels);
        }

        glGenBuffers(1, &octreesBuffer);
        glGenBuffers(1, &voxelsBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, octreesBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, nodes.size() * sizeof(GpuVoxelNode), nodes.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, octreesBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelsBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, voxels.size() * sizeof(Voxel), voxels.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelsBuffer);

        ShaderManager::Get().SetInt("voxelsCount", voxels.size());
    }

    void VoxelWorld::HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            isMouseMoving = false;
        }
    }
}