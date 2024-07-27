#include "InputSystem.h"
#include "../ImGUI/Backend/imgui_impl_sdl2.h"
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
	}
}