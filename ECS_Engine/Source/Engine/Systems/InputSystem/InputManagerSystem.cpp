#include "InputManagerSystem.h"
#include "backends/imgui_impl_sdl2.h"
#include "SDL.h"
#include "Systems/TaskManagerSystem.h"

#include <iostream>

namespace LKT
{
	InputManagerSystem& InputManagerSystem::InputManagerSystem::Get()
	{
		static InputManagerSystem instance;
		return instance;
	}

	InputManagerSystem::~InputManagerSystem()
	{
		TaskManagerSystem::Get().RemoveTask(this);
	}

	InputManagerSystem::InputManagerSystem()
	{
		TaskManagerSystem::Get().RegisterTask(this, &InputManagerSystem::ProcessInput, 0);
	}

	void InputManagerSystem::ProcessInput(float deltaTime)
	{
		RunEvents();
		
		SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);
	}

	void InputManagerSystem::RunEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT)
			{
				onCloseAppDelegate.Invoke();
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					onWindowResized.Invoke();
				}
			}

			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				mappedKeys[e.key.keysym.sym] = 1.0f;
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				mappedKeys[e.key.keysym.sym] = 0.0f;
			}

			//These events will be called continously!
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (!(e.button.button & pressedMouseButtons))
				{
					onMousePressed.Invoke(e.button);
					pressedMouseButtons &= e.button.button;
				}

				onMouseDown.Invoke(e.button);
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
				onMouseReleased.Invoke(e.button);
				pressedMouseButtons &= ~e.button.button;
			}
		}
	}

	float InputManagerSystem::GetHorizontalAxis()
	{
		return -mappedKeys[SDLK_a] + mappedKeys[SDLK_d];
	}

	float InputManagerSystem::GetVerticalAxis()
	{
		return mappedKeys[SDLK_w] + -mappedKeys[SDLK_s];;
	}
}