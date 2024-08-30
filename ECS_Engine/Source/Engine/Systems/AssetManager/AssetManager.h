#pragma once

#include "LazyAssetPtr.h"

#include <string>
#include <unordered_map>

namespace LKT
{
	class Asset;

	class AssetManager
	{
	public:
		static AssetManager& Get();

	private:
	};
}