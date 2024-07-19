#pragma once

#include "../Delegate.h"
#include "../Math/Numerals.h"

DECLARE_DELEGATE(OnCloseApp);
DECLARE_DELEGATE(OnWindowResized);
DECLARE_DELEGATE_TwoParams(OnWindowResizedParams, int32, int32);

class InputSystem
{
public:
	static InputSystem& Get();

	void ProcessInput();

	OnCloseApp onCloseAppDelegate;
	OnWindowResized onWindowResized;
	OnWindowResizedParams onWindowResizedParams;

private:
	InputSystem() = default;
};