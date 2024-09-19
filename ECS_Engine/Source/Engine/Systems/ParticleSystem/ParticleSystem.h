#pragma once

#include "ParticleSimType.h"
#include "Systems/AssetManager/ISerializable.h"

#include <string>
#include <vector>

namespace LKT
{
	class Mesh;
	class ParticleEmitter;
	class ShaderProgram;

	class ParticleSystem : public ISerializable
	{
	public:
		~ParticleSystem();
		ParticleSystem();

		EParticleSimType GetSimType() const { return simType; }

	protected:
		bool Serialize(std::ofstream &outStream) const override;
		bool Deserialize(std::ifstream &inStream) override;

		static void SerializeInitialData(std::ofstream &stream);

	private:
		void Simulate(float deltaTime);
		void Render();

		std::vector<ParticleEmitter *> emitters;

		void CreateEmitter();

		EParticleSimType simType = CPU;

		std::string shaderNameGPU = "Billboard_Unlit_GPU";
		std::string shaderNameCPU = "Billboard_Unlit_CPU";
		std::string computeShaderName = "Particles";		

		friend class ParticleSystemAsset;
		friend class ParticleSystemAssetViewer;
		friend class ParticleSystemAssetFactory;
	};
}