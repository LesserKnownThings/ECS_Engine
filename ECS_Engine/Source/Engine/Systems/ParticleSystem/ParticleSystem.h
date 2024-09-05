#pragma once

#include "ParticleSimType.h"
#include "Systems/AssetManager/ISerializable.h"

#include <string>
#include <vector>

namespace LKT
{
	class ParticleEmitter;
	class ShaderProgram;

	class ParticleSystem : public ISerializable
	{
	public:
		~ParticleSystem();
		ParticleSystem();

		void Simulate(float deltaTime);
		void Render();

	protected:
		bool Serialize(std::ostream &outStream) const override;
		bool Deserialize(std::ifstream &inStream) override;

	private:
		std::vector<ParticleEmitter *> emitters;

		void CreateEmitter();

		EParticleSimType simType = GPU;

		std::string shaderNameGPU = "Billboard_Unlit_GPU";
		std::string shaderNameCPU = "Billboard_Unlit_CPU";
		std::string computeShaderName = "Particles";

		friend class ParticleSystemAsset;
	};
}