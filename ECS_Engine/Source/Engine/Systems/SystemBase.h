#pragma once

#include "Entity.h"

#include <cstdint>

namespace LKT
{
	class SystemBase
	{
	public:
		virtual uint32_t CreateComponent(const Entity& e, void* componentData) = 0;
		virtual void CreateComponents(int32_t entityCount, Entity* entities, void* componentData) {}
	};
}