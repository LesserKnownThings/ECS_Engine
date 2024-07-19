#pragma once

#include "Math/Numerals.h"

#include <functional>

constexpr int32 DEFAULT_ALLOCATED_MEMORY = 32;

struct Entity
{
	uint32 id;

	bool operator==(const Entity& e) const
	{
		return id == e.id;
	}

	bool operator==(const uint32 other) const
	{
		return other == id;
	}
};

template<>
struct std::hash<Entity>
{
	size_t operator()(const Entity& e) const
	{
		return std::hash<uint32>()(e.id);
	}
};