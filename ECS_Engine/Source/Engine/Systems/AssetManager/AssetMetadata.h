#pragma once

#include <cstdint>
#include <string>

namespace LKT
{
    struct AssetMetadata
    {
        uint32_t type;
        int32_t pathSize;
        std::string path;

        /**
         * This data is usually not loaded unless required. It's still written on the .asset, but
         * it will only load when the engine requests it. An example for this would be loading
         * a mesh. When you need to load the mesh in the engine, the engine will load the data
         * and create the required data for rendering.
         */
        void *data = nullptr;
    };
}