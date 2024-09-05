#include "ParticleSystemAsset.h"
#include "Systems/ParticleSystem/ParticleSystem.h"

namespace LKT
{
    void ParticleSystemAsset::LoadAsset(const AssetPath &inPath)
    {
        Asset::LoadAsset(inPath);
    }

    bool ParticleSystemAsset::Serialize(std::ostream &outStream) const
    {
        bool success = Asset::Serialize(outStream);

        if (particleSystem != nullptr)
        {
            success &= particleSystem->Serialize(outStream);
        }

        return success;
    }

    bool ParticleSystemAsset::Deserialize(std::ifstream &inStream)
    {
        bool success = Asset::Deserialize(inStream);

        particleSystem = new ParticleSystem();
        success &= particleSystem->Deserialize(inStream);

        return success;
    }
}
