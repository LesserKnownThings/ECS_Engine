#include "DebugSystem.h"

#include <iostream>

void DebugSystem::Log(DebugLevel level, const std::string& message)
{
	switch (level)
	{
	case DebugLevel::Warning:
		std::cerr << "Warning: " << message << std::endl;
		break;
	case DebugLevel::Error:
		std::cerr << "Error: " << message << std::endl;
		break;
	default:
		std::cerr << "Log: " << message << std::endl;
		break;
	}
}