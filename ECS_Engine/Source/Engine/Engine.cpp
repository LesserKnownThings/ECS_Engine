#include "Engine.h"
#include "BuildMacros.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

#include "Systems/World/World.h"
#if GAME_ONLY
#include "Systems/World/GameWorld.h"
#endif

#include <unistd.h>

#include <iostream>

#include "Assets/Texture.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "EntityManager.h"
#include "SDL/SDL.h"
#include "SDL/SDL_mouse.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "Systems/MeshLoadingSystem.h"
#include "Systems/OpenGLSystem.h"
#include "Random.h"
#include "Systems/ResourceManagerSystem.h"
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
#if GAME_ONLY
        entryWorld = new GameWorld();
#elif
        entryWorld = new World();
#endif

        entryWorld->Initialize();

        uint32_t currentTicks = SDL_GetTicks();

        while (isRunning)
        {
            uint32_t newTicks = SDL_GetTicks();

            deltaTime = (newTicks - currentTicks) / 1000.0f;

            if (deltaTime > MAX_DELTA)
                deltaTime = MAX_DELTA;

            currentTicks = newTicks;

            const uint32_t frameStart = SDL_GetTicks();

            HandleSystems();

            const uint32_t frameDuration = SDL_GetTicks() - frameStart;

            if (FRAME_RATE > frameDuration)
            {
                SDL_Delay(FRAME_RATE);
            }
        }
    }

    void Engine::InitializeEngine()
    {
        if (InitializeCore())
        {
            AssetManager::Get().Initialize();
            InitializeUI();
            InitializeMisc();
            LoadShaders();

            isRunning = true;
            /**
             * This is running in a loop so the Uninitialize will be called after loop ends
             */
            RunEngine();
            UninitializeEngine();
        }
    }

    bool Engine::InitializeCore()
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
        InputManagerSystem::Get().onCloseAppDelegate.Bind(this, &Engine::HandleCloseEngine);
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
        TaskManagerSystem::Get().ExecuteTasks(PROCESS_HANDLE, deltaTime);
        OpenGLSystem::Get().Render();

        gcTimer -= deltaTime;
        if (gcTimer <= 0)
        {
            gcTimer = GC_DELAY;
            TaskManagerSystem::Get().ExecuteTasks(GC_HANDLE);
        }
    }

    void Engine::HandleCloseEngine()
    {
        isRunning = false;
    }
} // namespace LKT