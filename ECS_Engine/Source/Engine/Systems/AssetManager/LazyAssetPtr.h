#pragma once

#include "AssetPath.h"
#include "AssetFactory.h"
#include "Object.h"

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
					delete ptr;
				}

				ptr = nullptr;
			}
		}

		// This can only be made with an asset path, will probably check in the future to add support for default constructo too
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

		// No move operator, not reason to have this, use the move construct instead
		LazyAssetPtr &operator=(LazyAssetPtr &&other) = delete;
		// Copy operator will create a weak reference just like the copy construct!
		LazyAssetPtr &operator=(const LazyAssetPtr &other) = default;

		LazyAssetPtr<T> &StrongRef()
		{
			refCounter->Increment();
			LazyAssetPtr<T> *strongRefPtr = new LazyAssetPtr<T>(*this);
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
			ptr = AssetFactory::Get().LoadAsset<T>(path);
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