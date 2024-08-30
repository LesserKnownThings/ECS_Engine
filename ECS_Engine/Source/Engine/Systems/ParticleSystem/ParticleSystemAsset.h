#pragma once

#include "Systems/AssetManager/Asset.h"

#include <vector>

namespace LKT
{
	class ParticleSystem;
	class ParticleEmitter;

	class ParticleSystemAsset : public Asset
	{
	protected:
		void Serialize() override;
		void Deserialize() override;

	private:
		ParticleSystem* particleSystem = nullptr;
		std::vector<ParticleEmitter*> emitters;
	};
}