#pragma once

#include "Object.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#include <cstdint>

namespace LKT
{
    class Asset;
    class Camera;
    class EngineWindow;

    /**
     * The embedded viewport can be set with a width and height that will force it to stay that size
     * Or it can use the default construct which will fill the window while keeping the aspect ratio
     */
    class EmbeddedViewport : public Object
    {
    public:
        ~EmbeddedViewport();
        EmbeddedViewport();

        // The embedded viewport expects a window to render it, it's not using the Task system
        void Render();
        void HandleWindowResized(int32_t widht, int32_t height);

    private:
        EngineWindow *parent = nullptr;
        Camera *cameraRef = nullptr;
    };
}