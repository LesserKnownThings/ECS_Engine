#include "Engine.h"
#include "BuildMacros.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

#include <unistd.h>

#include <iostream>

#include "Assets/Texture.h"
#include "EntityManager.h"
#include "SDL/SDL.h"
#include "SDL/SDL_mouse.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "Systems/MeshLoadingSystem.h"
#include "Systems/OpenGLSystem.h"
#include "Random.h"
#include "Systems/RenderComponent.h"
#include "Systems/ResourceManagerSystem.h"
#include "Systems/TransformComponent.h"
#include "Systems/TransformSystem.h"

namespace LKT
{
    namespace
    {
        static void CheckSDLInit(uint32_t flags, const char *subsystem)
        {
            if (SDL_Init(flags) != 0)
            {
                printf("SDL_Init %s Error: %s\n", subsystem, SDL_GetError());
            }
            else
            {
                printf("%s initialized successfully.\n", subsystem);
                SDL_Quit();
            }
        }
    } // namespace

    const Engine *Engine::instance = nullptr;

    Engine::Engine()
    {
        instance = this;

        InitializeEngine();
    }

    const Engine *Engine::Get()
    {
        return instance;
    }

    bool Engine::IsPlayMode()
    {
        if (instance != nullptr)
        {
            instance->isPlayMode;
        }

        return false;
    }

    void Engine::RunEngine()
    {
        uint32_t currentTicks = SDL_GetTicks();

        while (isRunning)
        {
            uint32_t newTicks = SDL_GetTicks();

            deltaTime = (newTicks - currentTicks) / 1000.0f;

            if (deltaTime > MAX_DELTA)
                deltaTime = MAX_DELTA;

            currentTicks = newTicks;

            uint32_t frameStart = SDL_GetTicks();

            HandleSystems();

            uint32_t frameDuration = SDL_GetTicks() - frameStart;

            if (FRAME_RATE > frameDuration)
            {
                SDL_Delay(FRAME_RATE);
            }
        }
    }

    void Engine::InitializeEngine()
    {
        if (InitializeEverything())
        {
            InitializeMisc();
            LoadShaders();

            isRunning = true;
            RunEngine();
            UninitializeEngine();
        }
    }

    bool Engine::InitializeEverything()
    {
        return InitializeSDL() && InitializeOpenGL();
    }

    bool Engine::InitializeSDL()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0)
        {
            // LKT::CheckSDLInit(SDL_INIT_TIMER, "TIMER");
            // LKT::CheckSDLInit(SDL_INIT_AUDIO, "AUDIO");
            LKT::CheckSDLInit(SDL_INIT_VIDEO, "VIDEO");
            // LKT::CheckSDLInit(SDL_INIT_JOYSTICK, "JOYSTICK");
            // LKT::CheckSDLInit(SDL_INIT_HAPTIC, "HAPTIC");
            // LKT::CheckSDLInit(SDL_INIT_GAMECONTROLLER, "GAMECONTROLLER");
            LKT::CheckSDLInit(SDL_INIT_EVENTS, "EVENTS");

            return false;
        }

        return true;
    }

    bool Engine::InitializeOpenGL()
    {
        return OpenGLSystem::Get().InitializeSystem(1920, 1080);
    }

    void Engine::InitializeUI()
    {
        UIManager::Get().InitializeUI();
    }

    void Engine::InitializeMisc()
    {
        InitializeUI();

        AssetManager::Get().BuildAssetRegistry();

        InputManagerSystem::Get().onCloseAppDelegate.Bind(this, &Engine::HandleCloseEngine);

        int32_t entCount = 1;

        EntityResource res{entCount};
        glm::vec3 *position = new glm::vec3[entCount];
        glm::vec3 *rot = new glm::vec3[entCount];
        glm::vec3 *scale = new glm::vec3[entCount];

        for (int32_t i = 0; i < entCount; ++i)
        {
            position[i] = glm::vec3(Random::RandomRange(-250.0f, 250.0f), Random::RandomRange(-250.0f, 250.0f),
                                    Random::RandomRange(-250.0f, 250.0f));

            const float _scale = Random::RandomRange(10.0f, 20.0f);
            scale[i] = glm::vec3(_scale, _scale, _scale);
        }

        res.AddComponentResources<TransformComponentResource, RenderComponentResource>(
            TransformComponentResource{entCount, position, nullptr, scale}, RenderComponentResource{entCount});

        ResourceManagerSystem::Get().SpawnEntities<TransformComponentResource, RenderComponentResource>(res);
    }

    void Engine::UninitializeEngine()
    {
        UIManager::Get().UninitializeUI();
    }

    void Engine::LoadShaders()
    {
        ShaderManager::Get().LoadEngineShaders();
    }

    void Engine::HandleSystems()
    {
        TaskManagerSystem::Get().ExecuteTasks(std::nullopt, deltaTime);
        OpenGLSystem::Get().Render();
    }

    void Engine::HandleCloseEngine()
    {
        isRunning = false;
    }
} // namespace LKT