#pragma once

#include "ComponentFactory.h"
#include "Entity.h"
#include "Systems/MeshLoadingSystem.h"

#include <cstdint>
#include <string>

namespace LKT
{
	struct RenderComponent
	{
		int32_t instancesCount = 0;
		int32_t allocatedInstances = 0;

		void* buffer = nullptr;

		Entity* entity = nullptr;
		uint32_t* vao = nullptr;
		uint32_t* vbo = nullptr;
		uint32_t* ebo = nullptr;
		uint32_t* elementCount = nullptr;
		uint32_t* textureID = nullptr;
	};

	struct RenderComponentResource : public ComponentResource
	{
		RenderComponentResource(int32_t instances, uint32_t* textures = nullptr);
	};
}