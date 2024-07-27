#include "Engine.h"
#include "EntityManager.h"
#include "SDL.h"
#include "ShaderProgram.h"
#include "Systems/InputSystem.h"
#include "Systems/OpenGLSystem.h"

#include <iostream>

namespace
{
	static void CheckSDLInit(Uint32 flags, const char* subsystem)
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
}

const Engine* Engine::instance = nullptr;

Engine::~Engine()
{

}

Engine::Engine()
{
	instance = this;

	InitializeEngine();
}

const Engine* Engine::Get()
{
	return instance;
}

void Engine::RunEngine()
{
	uint32 currentTicks = SDL_GetTicks();

	while (isRunning)
	{
		uint32 newTicks = SDL_GetTicks();

		deltaTime = (newTicks - currentTicks) / 1000.0f;

		if (deltaTime > MAX_DELTA)
			deltaTime = MAX_DELTA;

		currentTicks = newTicks;

		uint32 frameStart = SDL_GetTicks();

		HandleInput();
		HandleSystems();

		uint32 frameDuration = SDL_GetTicks() - frameStart;

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

		isRunning = true;
		RunEngine();
	}
}

bool Engine::InitializeEverything()
{
	return InitializeSDL() && InitializeOpenGL();
}

bool Engine::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		::CheckSDLInit(SDL_INIT_TIMER, "TIMER");
		::CheckSDLInit(SDL_INIT_AUDIO, "AUDIO");
		::CheckSDLInit(SDL_INIT_VIDEO, "VIDEO");
		::CheckSDLInit(SDL_INIT_JOYSTICK, "JOYSTICK");
		::CheckSDLInit(SDL_INIT_HAPTIC, "HAPTIC");
		::CheckSDLInit(SDL_INIT_GAMECONTROLLER, "GAMECONTROLLER");
		::CheckSDLInit(SDL_INIT_EVENTS, "EVENTS");

		return false;
	}

	return true;
}

bool Engine::InitializeOpenGL()
{
	if (OpenGLSystem* openGLSystem = SystemManager::Get().GetSystem<OpenGLSystem>(typeid(OpenGLSystem)))
	{
		return openGLSystem->InitializeSystem(640, 480);
	}

	return false;
}

#include "Systems/ResourceManagerSystem.h"

void Engine::InitializeMisc()
{
	InputSystem::Get().onCloseAppDelegate.Bind(this, &Engine::HandleCloseEngine);

	uint16 compTypes = (uint16)ComponentType::TransformComponent | (uint16)ComponentType::RenderComponent;

	int32 componentsCount = 3;

	ComponentData* components = new ComponentData[componentsCount];	
	
	components[0].type = ComponentType::TransformComponent;
	components[1].type = ComponentType::RenderComponent;
	components[2].type = ComponentType::PhysicsComponent;

	EntityResource es(10000, componentsCount, compTypes, components);
	ResourceManagerSystem::Get().SpawnEntities(es);
}

void Engine::HandleInput()
{
	InputSystem::Get().ProcessInput();
}

void Engine::HandleSystems()
{
	for (const auto& it : SystemManager::Get().GetAllSystems())
	{
		it.second->Render(); //Rendering before process because of physics
		it.second->Process(deltaTime);		
	}
}

void Engine::HandleCloseEngine()
{
	isRunning = false;
}