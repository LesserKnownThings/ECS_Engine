#include "ParticleSystemAsset.h"
#include "Systems/ParticleSystem/ParticleSystem.h"

namespace LKT
{
    void ParticleSystemAsset::UnloadAsset()
    {
        delete particleSystem;
        particleSystem = nullptr;
    }

    void ParticleSystemAsset::LoadAsset(void *buffer)
    {
        particleSystem = new ParticleSystem();
        // particleSystem->Deserialize(inStream);
    }
}
