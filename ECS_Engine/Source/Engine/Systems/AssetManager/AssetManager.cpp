#include "AssetManager.h"

namespace LKT
{
	AssetManager& AssetManager::Get()
	{
		static AssetManager instance;
		return instance;
	}
}