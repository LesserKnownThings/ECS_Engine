#pragma once

#include "../Entity.h"
#include "../Math/Transforms.h"

#include <unordered_map>

class TransformSystem
{
	struct TransformComponentData
	{
		int32 instancesCount;
		int32 allocatedInstances;

		void* buffer;

		Vector3* position;
		Vector3* eulerAngles;
		Vector3* scale;		

		Entity* entity;	
	};

	TransformComponentData transformComponentData;

public:
	TransformSystem();
	~TransformSystem();

	uint32 AddComponent(const Entity& e);

	bool EntityHasComponent(const Entity& e) const;
	bool GetComponent(const Entity& e, uint32& outComponent) const;

private:
	void AllocateMemory(int32 size);

	std::unordered_map<uint32, uint32> instances;
};