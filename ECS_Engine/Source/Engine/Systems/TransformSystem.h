#pragma once

#include "SystemBase.h"
#include "ResourceManagerSystem.h"
#include "TransformComponent.h"
#include "glm/glm.hpp"

#include <iostream>
#include <unordered_map>

namespace LKT
{
	class TransformSystem : public SystemBase
	{
	public:
		TransformSystem();
		~TransformSystem();

		static TransformSystem& Get();

		uint32_t CreateComponent(const Entity& e, void* componentData) override;
		void CreateComponents(int32_t entityCount, Entity* entities, void* componentData) override;
		void Process(float deltaTime);

		bool EntityHasComponent(const Entity& e) const;
		bool GetComponent(const Entity& e, uint32_t& outComponent) const;

		bool GetPosition(const Entity& e, glm::vec3& outPos) const;

		void SetPosition(const Entity& e, const glm::vec3& position);
		void SetRotation(const Entity& e, const glm::vec3& eulerAngles);
		void SetScale(const Entity& e, const glm::vec3& scale);

		void SetPosition(uint32_t component, const glm::vec3& position);
		void SetRotation(uint32_t component, const glm::vec3& eulerAngles);
		void SetScale(uint32_t component, const glm::vec3& scale);

		void Move(const Entity& e, const glm::vec3& direction);
		void Rotate(const Entity& e, const glm::vec3& axis);

		void Move(uint32_t component, const glm::vec3& direction);
		void Rotate(uint32_t component, const glm::vec3& axis);

		bool GetModel(const Entity& e, glm::mat4& outModel) const;

	private:
		void AllocateMemory(int32_t size);

		void ProcessComponents(float deltaTime, uint32_t startIndex, uint32_t endIndex);

		TransformComponent transformComponent;
		std::unordered_map<uint32_t, uint32_t> instances;
	};

	REGISTER_COMPONENT(TransformSystem, TransformComponentResource)
}