#pragma once

#include "Entity.h"

#include <cstdint>
#include <typeindex>

namespace LKT
{
	class SystemBase
	{
	public:
		virtual void CreateComponents(int32_t entityCount,
									  Entity *entities,
									  const std::type_index &type,
									  void *commonData,
									  void *componentData) = 0;
	};
}