#pragma once

#include "Math/Numerals.h"

class InputSystem;
class OpenGLSystem;

constexpr int32 FRAME_RATE = 16; // 60 fps
constexpr float MAX_DELTA = 0.05f;

class Engine
{
public:
	~Engine();
	Engine();

	static const Engine* Get();

	float GetDeltaTime() const { return deltaTime; }

private:
	void RunEngine();	

	void InitializeEngine();
	bool InitializeEverything();
	bool InitializeSDL();
	bool InitializeOpenGL();
	void InitializeMisc();

	void HandleInput();
	void HandleSystems();

	void HandleCloseEngine();

	bool isRunning = false;
	float deltaTime = 0.0f;

	static const Engine* instance;
};