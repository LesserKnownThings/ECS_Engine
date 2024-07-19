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

private:
	void RunEngine();	

	void InitializeEngine();
	bool InitializeEverything();
	bool InitializeSDL();
	bool InitializeOpenGL();
	void InitializeMisc();

	void HandleInput();
	void HandleRendering();

	void HandleCloseEngine();

	bool isRunning = false;
	float deltaTime = 0.0f;

	//**********Systems
	OpenGLSystem* openGLSystem = nullptr;
};