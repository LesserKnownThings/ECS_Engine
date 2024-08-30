#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace LKT
{
	constexpr int32_t DEFAULT_ALLOCATED_MEMORY = 32;

	struct Entity
	{
		uint32_t id;

		bool operator==(const Entity& e) const
		{
			return id == e.id;
		}

		bool operator==(const uint32_t other) const
		{
			return other == id;
		}
	};
}

template<>
struct std::hash<LKT::Entity>
{
	size_t operator()(const LKT::Entity& e) const
	{
		return std::hash<uint32_t>()(e.id);
	}
};