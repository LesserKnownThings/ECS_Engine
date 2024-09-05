#pragma once

#include "ComponentFactory.h"
#include "Entity.h"
#include "glm/glm.hpp"

#include <cstdint>
#include <iostream>

namespace LKT
{
	struct TransformComponent
	{
		int32_t instancesCount = 0;
		int32_t allocatedInstances = 0;

		void* buffer = nullptr;

		glm::vec3* position = nullptr;
		glm::vec3* eulerAngles = nullptr;
		glm::vec3* scale = nullptr;

		Entity* entity = nullptr;
	};


	struct TransformComponentResource : public ComponentResource
	{
		TransformComponentResource(int32_t instances, glm::vec3* position = nullptr, glm::vec3* eulerAngles = nullptr, glm::vec3* scale = nullptr);
	};
}