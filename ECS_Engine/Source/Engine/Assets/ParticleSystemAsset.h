#pragma once

#include "Systems/AssetManager/Asset.h"

namespace LKT
{
    class ParticleSystem;

    class ParticleSystemAsset : public Asset
    {
    protected:
        virtual void LoadAsset(const AssetPath &inPath) override;

        bool Serialize(std::ostream &outStream) const override;
        bool Deserialize(std::ifstream &inStream) override;

    private:
        ParticleSystem *particleSystem = nullptr;
    };
}