#pragma once

#include "UniqueID.h"

#include <string>

namespace LKT
{
	/// <summary>
	/// This is just an utility class
	/// The main purpose of the engine is to avoid using objects, but I'm still using them for UI and particle systems
	/// Everything else in the engine should not use objects, so if an entity is using an object somehow that's wrong!!
	/// </summary>

	class Object
	{
	public:
		virtual ~Object();
		Object();

		const UniqueID &GetID() const { return id; }

		bool operator()(const Object *lhs, const Object *rhs) const
		{
			return lhs->id == rhs->id;
		}

		void OverrideID(const UniqueID &inID);

	private:
		UniqueID id;
	};
}

template <>
struct std::hash<LKT::Object>
{
	size_t operator()(const LKT::Object &obj) const
	{
		return std::hash<LKT::UniqueID>{}(obj.GetID());
	}
};