#include "LazyAssetPtr.h"
#include "AssetManager.h"

namespace LKT
{
    // LazyAssetPtr::~LazyAssetPtr()
    // {
    //     if (refCounter != nullptr && isStrong)
    //     {
    //         refCounter->Decrement();

    //         if (refCounter->counter <= 0)
    //         {
    //             ptr->UnloadAsset();
    //             delete ptr;
    //         }

    //         ptr = nullptr;
    //     }
    // }

    // LazyAssetPtr::LazyAssetPtr(const AssetPath &inPath)
    //     : ptr(nullptr), refCounter(new RefCounter{0}), path(inPath) {}

    // LazyAssetPtr::LazyAssetPtr(const LazyAssetPtr &other)
    //     : ptr(other.ptr), refCounter(other.refCounter), isStrong(other.isStrong), path(other.path)
    // {
    // }

    // LazyAssetPtr::LazyAssetPtr(LazyAssetPtr &&other) noexcept
    //     : refCounter(std::move(other.refCounter)), path(std::move(other.path))
    // {
    //     other.refCounter = nullptr;
    // }

    // LazyAssetPtr &LazyAssetPtr::StrongRef() const
    // {
    //     refCounter->Increment();
    //     LazyAssetPtr *strongRefPtr = new LazyAssetPtr(*this);
    //     strongRefPtr->LoadAsset();
    //     strongRefPtr->isStrong = true;
    //     return *strongRefPtr;
    // }

    // void LazyAssetPtr::LoadAsset()
    // {
    //     if (ptr == nullptr)
    //     {
    //         ptr = AssetManager::LoadAsset(path);
    //     }
    // }
}