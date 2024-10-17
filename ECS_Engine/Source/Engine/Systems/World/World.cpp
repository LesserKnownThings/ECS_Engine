#include "World.h"
#include "Camera.h"
#include "Systems/OpenGLSystem.h"

namespace LKT
{
    World::~World()
    {
        delete camera;
    }

    void World::Initialize()
    {
        camera = OpenGLSystem::RequestCamera(glm::vec3(0.f, 0.f, 30.0f));       
    }

    void World::Uninitialize()
    {
    }
}