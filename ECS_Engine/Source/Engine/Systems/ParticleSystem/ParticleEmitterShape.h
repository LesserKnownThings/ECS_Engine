#pragma once

#include "../../Math/Numerals.h"
#include "../../Math/Transforms.h"
#include "ParticleShapeData.h"

#include <functional>

namespace LKT
{
	using ShapeDataFunc = std::function<void(const Vector3& position, const Vector3& velocity)>;

	class ParticleEmitterShape
	{
	public:
		virtual void GetData(ShapeDataFunc func) = 0;
		virtual void UpdateFromUI(const std::string& key, ShapeDataVariant value) = 0;
		virtual std::unordered_map<std::string, ShapeDataVariant> GetUIData() = 0;
	};
}