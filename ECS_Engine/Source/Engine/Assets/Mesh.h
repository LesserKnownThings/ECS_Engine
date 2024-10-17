#pragma once

#include "Components/RenderComponent.h"
#include "Systems/AssetManager/Asset.h"

#include <string>

namespace LKT
{
    class Mesh : public Asset
    {
    public:
        void UnloadAsset() override;

        const RenderComponentObject &GetRenderComponentObject() const;

    protected:
        void LoadAsset(void *buffer) override;

    private:
        RenderComponentObject rco;
    };
}