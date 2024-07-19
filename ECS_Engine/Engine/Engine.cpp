#include "Engine.h"
#include "EntityManager.h"
#include "SDL.h"
#include "Systems/InputSystem.h"
#include "Systems/MeshLoadingSystem.h"
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

Engine::~Engine()
{
	delete openGLSystem;
	openGLSystem = nullptr;
}

Engine::Engine()
{
	InitializeEngine();
}

glm::mat4 pos = glm::mat4(1.0f);

void Engine::RunEngine()
{
	uint32 previousFrameTime = SDL_GetTicks();

	while (isRunning)
	{
		uint32 currentTime = SDL_GetTicks();
		uint32 frameDuration = currentTime - previousFrameTime;

		previousFrameTime = currentTime;

		deltaTime = frameDuration / 1000.0f;

		if (deltaTime > MAX_DELTA)
			deltaTime = MAX_DELTA;

		HandleInput();
		HandleRendering();

		ShaderProgram& sp = openGLSystem->GetDefaultShader();
		
		pos = glm::rotate(pos, TO_RADIANS(1.f), glm::vec3(0.0f, 1.0f, 0.0f));
		sp.SetMat4f("model", DATA(pos));

		SDL_Delay(FRAME_RATE);
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
	if (openGLSystem = new OpenGLSystem())
	{
		return openGLSystem->InitializeSystem(640, 480);
	}

	return false;
}

void Engine::InitializeMisc()
{
	InputSystem::Get().onCloseAppDelegate.Bind(this, &Engine::HandleCloseEngine);

	MeshLoadingSystem& mls = MeshLoadingSystem::Get();

	const std::string coolMeshPath = "Data/CoolMesh.obj";

	mls.ImportMesh(coolMeshPath);
	
	Entity e = EntityManager::Get().CreateEntity();

	uint32 comp = openGLSystem->CreateComponent(e);
	openGLSystem->SetComponentMesh(coolMeshPath, e);

	ShaderProgram& sp = openGLSystem->GetDefaultShader();

	sp.Use();

	Mat4 projection = glm::perspective(TO_RADIANS(45.0f), (640.0f / 480.0f), 0.1f, 5000.0f);

	Mat4 view = glm::lookAt(
		Vector3(0.0f, 0.0f, 100.0f),
		Vector3(0.0f) + Vector3(0.0f, 0.0f, 1.0f),
		Vector3(0.0f, 1.0f, 0.0f)
	);

	Mat4 model = Mat4(1.0f);

	sp.SetMat4f("projection", DATA(projection));
	sp.SetMat4f("view", DATA(view));
	sp.SetMat4f("model", DATA(model));
}

void Engine::HandleInput()
{
	InputSystem::Get().ProcessInput();
}

void Engine::HandleRendering()
{
	openGLSystem->PreRender();
	openGLSystem->Render();
	openGLSystem->PostRender();
}

void Engine::HandleCloseEngine()
{
	isRunning = false;
}