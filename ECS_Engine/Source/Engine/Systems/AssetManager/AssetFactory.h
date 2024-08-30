#pragma once

#include "Asset.h"

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace LKT
{
	using FactoryFunc = std::function<Asset* (const std::string& path)>;

	class AssetFactory
	{
	public:
		static AssetFactory& Get();

		template<typename T>
		T* LoadAsset(const std::string path);

	private:
		AssetFactory();

		std::unordered_map<std::type_index, FactoryFunc> factories;
	};

	template<typename T>
	inline T* AssetFactory::LoadAsset(const std::string path)
	{
		const auto it = factories.find(typeid(T));

		if (it != factories.end())
		{
			return reinterpret_cast<T*>(it->second(path));
		}

		return nullptr;
	}
}