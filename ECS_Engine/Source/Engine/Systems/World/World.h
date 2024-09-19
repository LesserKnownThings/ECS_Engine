#pragma once

#include "BuildMacros.h"
#include "Object.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#include <vector>

namespace LKT
{
	class Asset;

	class World : public Object
	{
	public:
		World();
		void GiveAsset(LazyAssetPtr<Asset> &strongRefAsset);

	private:
		void Render();
		void Process(float deltaTime);

		std::vector<LazyAssetPtr<Asset>> assets;
	};
}