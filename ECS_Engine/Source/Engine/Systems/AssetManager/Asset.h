#pragma once

#include "Object.h"
#include "ISerializable.h"

#include <cstring>
#include <string>

namespace LKT
{
	class Asset : public Object, public ISerializable
	{
	public:
		virtual ~Asset() = default;

		virtual void LoadAsset(void *buffer) = 0;
		virtual void UnloadAsset() = 0;

		virtual bool Serialize(std::ofstream &outStream) const {}
		virtual bool Deserialize(std::ifstream &inStream) {}

	protected:
		Asset() = default;
		friend class AssetFactoryManager;
	};
}