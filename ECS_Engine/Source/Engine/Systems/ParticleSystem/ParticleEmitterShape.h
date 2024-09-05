#pragma once

#include "ParticleShapeData.h"
#include "glm/glm.hpp"

#include <functional>

namespace LKT
{
	using ShapeDataFunc = std::function<void(const glm::vec3& position, const glm::vec3& velocity)>;

	class ParticleEmitterShape
	{
	public:
		virtual void GetData(ShapeDataFunc func) = 0;
		virtual void UpdateFromUI(const std::string& key, ShapeDataVariant value) = 0;
		virtual std::unordered_map<std::string, ShapeDataVariant> GetUIData() = 0;
	};
}