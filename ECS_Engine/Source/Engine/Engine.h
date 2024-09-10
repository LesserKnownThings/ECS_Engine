#pragma once

#include <cstdint>

namespace LKT
{
	class InputSystem;
	class OpenGLSystem;

	constexpr int32_t FRAME_RATE = 16; // 60 fps
	constexpr float MAX_DELTA = .016f;

	class Engine
	{
	public:
		~Engine() = default;
		Engine();

		static const Engine *Get();

		float GetDeltaTime() const { return deltaTime; }

		static bool IsPlayMode();

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

		// True when we're in a build or play mode in the editor
		bool isPlayMode = false;
		float deltaTime = 0.0f;

		static const Engine *instance;
	};
}