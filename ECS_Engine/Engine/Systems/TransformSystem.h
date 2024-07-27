#pragma once

#include "../Entity.h"
#include "../Math/Transforms.h"
#include "SystemBase.h"
#include "SystemManager.h"
#include "ResourceManagerSystem.h"
#include "ComponentsMetadata.h"

#include <iostream>
#include <unordered_map>

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

class TransformSystem : public SystemBase
{
public:
	TransformSystem();
	~TransformSystem();
		
	virtual uint32 CreateComponent(const Entity& e, void* componentData) override;
	virtual void Process(float deltaTime) override;

	bool EntityHasComponent(const Entity& e) const;
	bool GetComponent(const Entity& e, uint32& outComponent) const;

	void SetPosition(const Entity& e, const Vector3& position);
	void SetRotation(const Entity& e, const Vector3& eulerAngles);
	void SetScale(const Entity& e, const Vector3& scale);

	void SetPosition(uint32 component, const Vector3& position);
	void SetRotation(uint32 component, const Vector3& eulerAngles);
	void SetScale(uint32 component, const Vector3& scale);

	void Move(const Entity& e, const Vector3& direction);
	void Rotate(const Entity& e, const Vector3& axis);

	void Move(uint32 component, const Vector3& direction);
	void Rotate(uint32 component, const Vector3& axis);

private:
	void AllocateMemory(int32 size);

	void ProcessComponents(float deltaTime, uint32 startIndex, uint32 endIndex);

	TransformComponentData transformComponentData;
	std::unordered_map<uint32, uint32> instances;
};

REGISTER_SYSTEM(TransformSystem)
REGISTER_CREATION_FUNCTION(TransformSystem, ComponentType::TransformComponent)