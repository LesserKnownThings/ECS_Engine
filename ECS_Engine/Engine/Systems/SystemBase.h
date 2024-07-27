#pragma once

#include "../Entity.h"
#include "../Math/Numerals.h"

class SystemBase
{
public:
	virtual uint32 CreateComponent(const Entity& e, void* componentData) { return 0; }

	virtual void Process(float deltaTime) {}
	virtual void Render() {}
};