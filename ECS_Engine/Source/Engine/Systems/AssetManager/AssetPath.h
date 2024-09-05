#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace LKT
{
    struct AssetPath
    {
        AssetPath() = default;
        AssetPath(const std::filesystem::path &inFullPath);

        AssetPath(const AssetPath &other)
            : fullPath(other.fullPath),
              assetName(other.assetName),
              extension(other.extension),
              subFolders(other.subFolders)
        {
        }

        AssetPath(AssetPath &&other) noexcept
            : fullPath(std::move(other.fullPath)),
              assetName(std::move(other.assetName)),
              extension(std::move(other.extension)),
              subFolders(std::move(other.subFolders))
        {
        }

        AssetPath &operator=(AssetPath &&other) noexcept
        {
            if (this != &other)
            {
                fullPath = std::move(other.fullPath);
                assetName = std::move(other.assetName);
                extension = std::move(other.extension);
                subFolders = std::move(other.subFolders);
            }

            return *this;
        }

        AssetPath &operator=(const AssetPath &other) = default;

        bool operator==(const AssetPath &other) const
        {
            return other.fullPath == fullPath;
        }

        std::string fullPath;
        std::string assetName;
        std::string extension;
        std::vector<std::string> subFolders;
    };

    struct AssetPathHash
    {
        size_t operator()(const AssetPath &assetPath) const
        {
            std::hash<std::string> h;
            return h(assetPath.fullPath);
        }
    };
}