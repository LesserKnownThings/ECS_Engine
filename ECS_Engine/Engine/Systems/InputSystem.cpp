#include "InputSystem.h"
#include "SDL.h"

InputSystem& InputSystem::Get()
{
	static InputSystem instance;
	return instance;
}

void InputSystem::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
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
	}
}