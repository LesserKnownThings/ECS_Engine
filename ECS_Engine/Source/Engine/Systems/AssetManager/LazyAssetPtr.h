#pragma once

#include "Asset.h"
#include "AssetManager.h"
#include "AssetPath.h"

#include <cassert>
#include <iostream>
#include <string>

namespace LKT
{
	template <typename T>
	class LazyAssetPtr
	{
		struct RefCounter
		{
			RefCounter(int32_t inCounter)
				: counter(inCounter) {}

			void Increment()
			{
				++counter;
			}

			void Decrement()
			{
				--counter;
			}

			int32_t counter = 0;
		};

	public:
		~LazyAssetPtr()
		{
			if (refCounter != nullptr && isStrong)
			{
				refCounter->Decrement();

				if (refCounter->counter <= 0)
				{
					ptr->UnloadAsset();
					delete ptr;
				}

				ptr = nullptr;
			}
		}

		LazyAssetPtr()
			: ptr(nullptr), refCounter(new RefCounter{0}) {}

		LazyAssetPtr(const AssetPath &inPath)
			: ptr(nullptr), refCounter(new RefCounter{0}), path(inPath) {}

		// Creates a weak ref that doesn't ensure the ptr is valid!
		LazyAssetPtr(const LazyAssetPtr &other)
			: ptr(other.ptr), refCounter(other.refCounter), isStrong(other.isStrong), path(other.path)
		{
		}

		// The isStrong is not moved since a move should not create strong references!
		LazyAssetPtr(LazyAssetPtr &&other) noexcept
			: refCounter(std::move(other.refCounter)), path(std::move(other.path))
		{
			other.refCounter = nullptr;
		}

		// Since we now have a default construct we need to have a move construct so we can move the strong ref
		LazyAssetPtr &operator=(LazyAssetPtr &&other)
		{
			if (this != &other)
			{
				assert(("A strong reference cannot be assigned any new value!", !isStrong));

				ptr = other.ptr;
				other.ptr = nullptr;

				refCounter = other.refCounter;
				other.refCounter = nullptr;

				isStrong = other.isStrong;
				other.isStrong = false;
			}

			return *this;
		}

		LazyAssetPtr &operator=(const LazyAssetPtr &other)
		{
			if (this != &other)
			{
				assert(("A strong reference cannot be assigned any new value!", !isStrong));
				assert(("Cannot copy a strong reference, please use move instead!", !other.isStrong));
				ptr = other.ptr;
				refCounter = other.refCounter;
			}

			return *this;
		}

		// Creates a strong reference and loads the asset if it wasn't already
		LazyAssetPtr<T> &StrongRef() const
		{
			refCounter->Increment();
			LazyAssetPtr<T> *strongRefPtr = new LazyAssetPtr<T>(*this);
			strongRefPtr->LoadAsset();
			strongRefPtr->isStrong = true;
			return *strongRefPtr;
		}

		void LoadAsset();

		template <class C>
		C *Get();

		const std::string &GetName() const { return path.assetName; }
		const std::string &GetExtension() const { return path.extension; }

		const AssetPath &GetPath() const { return path; }

	private:
		T *ptr = nullptr;
		RefCounter *refCounter = nullptr;
		bool isStrong = false;

		AssetPath path;
	};

	template <typename T>
	struct LazyAssetPtrHash
	{
		std::size_t operator()(const LazyAssetPtr<T> &ptr)
		{
			std::hash<std::string> hashable;
			return ptr.path;
		}
	};

	template <typename T>
	void LazyAssetPtr<T>::LoadAsset()
	{
		if (ptr == nullptr)
		{
			ptr = AssetManager::LoadAsset(path);
		}
	}

	template <typename T>
	template <class C>
	inline C *LazyAssetPtr<T>::Get()
	{
		if (ptr != nullptr)
		{
			return reinterpret_cast<C *>(ptr);
		}

		std::cerr << "Error:: Asset not loaded, load asset before trying to call Get!" << std::endl;
		return nullptr;
	}
}