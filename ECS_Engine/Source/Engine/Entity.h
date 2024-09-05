#pragma once

#include <cstdint>
#include <vector>

namespace LKT
{
	constexpr int32_t DEFAULT_ALLOCATED_MEMORY = 32;

	struct Entity
	{
		Entity() : id(0) {}
		Entity(uint32_t inId)
			: id(inId) {}

		bool operator==(const Entity &e) const
		{
			return id == e.id;
		}

		uint32_t id;
	};
}

template <>
struct std::hash<LKT::Entity>
{
	size_t operator()(const LKT::Entity &e) const
	{
		return std::hash<uint32_t>()(e.id);
	}
};