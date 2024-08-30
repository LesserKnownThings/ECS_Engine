#pragma once

#include <string>
#include <variant>
#include <unordered_map>

using ShapeDataVariant = std::variant<float, bool>;

namespace LKT
{
	struct ParticleShapeData
	{
	public:
		std::string shapeName;

		int32_t funcRef;
		int32_t paramsRef;
		std::unordered_map<std::string, ShapeDataVariant> params;
	};
}