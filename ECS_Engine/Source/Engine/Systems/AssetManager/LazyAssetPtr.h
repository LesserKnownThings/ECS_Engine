#pragma once

#include "AssetFactory.h"
#include "Object.h"

#include <string>

namespace LKT
{
	template <typename T>
	class LazyAssetPtr
	{
		struct Ref
		{
			T* ptr = nullptr;
			int32_t counter = 1;

			Ref& operator=(Ref&& other) = delete;
			Ref& operator=(const Ref& other)
			{
				if (this != &other)
				{
					ptr = other.ptr;
					counter = other.counter;
				}

				return *this;
			}
		};

	public:
		~LazyAssetPtr();
		LazyAssetPtr(const std::string& inPath);

		LazyAssetPtr& operator=(const LazyAssetPtr& other)
		{			
			if (this != &other)
			{
				ref->counter++;
				ref = other.ref;
			}

			return *this;
		}

		T* LoadAsset();

	private:
		LazyAssetPtr(const LazyAssetPtr& other) = delete;
		LazyAssetPtr(LazyAssetPtr&& other) = delete;
		
		LazyAssetPtr& operator=(LazyAssetPtr&& other) = delete;

		int32_t refCounter = 0;

		std::string fullPath;

		Ref* ref;
	};

	template<typename T>
	inline LazyAssetPtr<T>::~LazyAssetPtr()
	{
		ref->counter--;

		if (ref->counter <= 0 && ref->ptr != nullptr)
		{
			delete ref->ptr;
		}

		ref = nullptr;
	}

	template<typename T>
	inline LazyAssetPtr<T>::LazyAssetPtr(const std::string& inPath)
		: fullPath(inPath), ref(new Ref())
	{
	}

	template<typename T>
	inline T* LazyAssetPtr<T>::LoadAsset()
	{
		if (ref->ptr == nullptr)
		{
			ref->ptr = AssetFactory::Get().LoadAsset<T>(fullPath);
		}

		return ref->ptr;
	}
}