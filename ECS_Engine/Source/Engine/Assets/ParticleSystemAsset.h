#pragma once

#include "Systems/AssetManager/Asset.h"

namespace LKT
{
    class ParticleSystem;

    class ParticleSystemAsset : public Asset
    {
    public:
        ParticleSystem *GetParticleSystem() const { return particleSystem; }
        void UnloadAsset() override;

    protected:
        void LoadAsset(void *buffer) override;

    private:
        ParticleSystem *particleSystem = nullptr;
    };
}