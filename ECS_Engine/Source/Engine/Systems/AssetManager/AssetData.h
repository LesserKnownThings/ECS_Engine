#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetPath.h"
#include "LazyAssetPtr.h"

namespace LKT
{
    struct AssetData
    {
        AssetData(const AssetPath &assetPath, const AssetMetadata &inMetadata)
            : metadata(inMetadata), asset(LazyAssetPtr<Asset>(assetPath)) {}

        AssetMetadata metadata;
        LazyAssetPtr<Asset> asset;
    };
}