#include "ParticleSystemAsset.h"
#include "Systems/ParticleSystem/ParticleSystem.h"

namespace LKT
{
    void ParticleSystemAsset::UnloadAsset()
    {
        delete particleSystem;
        particleSystem = nullptr;
    }

    bool ParticleSystemAsset::Serialize(std::ofstream &outStream) const
    {
        return particleSystem->Serialize(outStream);
    }

    bool ParticleSystemAsset::Deserialize(std::ifstream &inStream)
    {
        particleSystem = new ParticleSystem();
        return particleSystem->Deserialize(inStream);
    }
}
