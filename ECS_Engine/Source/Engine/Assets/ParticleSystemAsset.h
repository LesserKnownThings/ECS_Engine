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
        // It's empty since this asset saves everything to the binary instead of the metadata portion
        void LoadAsset(void *buffer) override {}

        bool Serialize(std::ofstream &outStream) const override;
        bool Deserialize(std::ifstream &inStream) override;

    private:
        ParticleSystem *particleSystem = nullptr;
    };
}