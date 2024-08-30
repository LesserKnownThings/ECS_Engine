#pragma once

#include "Math/Numerals.h"

namespace LKT
{
	class InputSystem;
	class OpenGLSystem;

	constexpr int32 FRAME_RATE = 16; // 60 fps
	constexpr float MAX_DELTA = .016f;

	class Engine
	{
	public:
		~Engine() = default;
		Engine();

		static const Engine* Get();

		float GetDeltaTime() const { return deltaTime; }

	private:
		void RunEngine();

		void InitializeEngine();
		bool InitializeEverything();
		bool InitializeSDL();
		bool InitializeOpenGL();
		void InitializeUI();
		void InitializeMisc();

		void UninitializeEngine();

		void LoadShaders();

		void HandleSystems();

		void HandleCloseEngine();

		bool isRunning = false;
		float deltaTime = 0.0f;

		static const Engine* instance;
	};
}