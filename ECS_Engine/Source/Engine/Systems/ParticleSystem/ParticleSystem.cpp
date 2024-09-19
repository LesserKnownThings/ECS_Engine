#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Systems/ShaderSystem/ShaderProgram.h"
#include "Systems/ShaderSystem/ShaderManager.h"

namespace LKT
{
	ParticleSystem::~ParticleSystem()
	{
		for (ParticleEmitter *emitter : emitters)
		{
			delete emitter;
		}

		emitters.clear();
	}

	ParticleSystem::ParticleSystem()
	{
		CreateEmitter();
	}

	void ParticleSystem::Simulate(float deltaTime)
	{
		if (simType == GPU)
		{
			ShaderManager::Get().ActivateShader(computeShaderName);
		}

		for (ParticleEmitter *emitter : emitters)
		{
			emitter->Simulate(deltaTime);
		}
	}

	void ParticleSystem::Render()
	{
		switch (simType)
		{
		case CPU:
			ShaderManager::Get().ActivateShader(shaderNameCPU);
			break;
		default:
			ShaderManager::Get().ActivateShader(shaderNameGPU);
			break;
		}

		for (ParticleEmitter *emitter : emitters)
		{
			emitter->Render();
		}
	}

	bool ParticleSystem::Serialize(std::ofstream &outStream) const
	{
		bool success = true;

		outStream.write(reinterpret_cast<const char *>(&simType), sizeof(EParticleSimType));

		const int32_t emittersCount = emitters.size();

		outStream.write(reinterpret_cast<const char *>(&emittersCount), sizeof(int32_t));

		for (const ParticleEmitter *pm : emitters)
		{
			success &= pm->Serialize(outStream);
		}

		return success;
	}

	bool ParticleSystem::Deserialize(std::ifstream &inStream)
	{
		bool success = true;

		inStream.read(reinterpret_cast<char *>(&simType), sizeof(EParticleSimType));

		int32_t emittersCount = 0;
		inStream.read(reinterpret_cast<char *>(&emittersCount), sizeof(int32_t));

		for (int32_t i = 0; i < emittersCount; ++i)
		{
			CreateEmitter();
			emitters[i]->Deserialize(inStream);
		}

		return success;
	}

	void ParticleSystem::SerializeInitialData(std::ofstream &stream)
	{
		const EParticleSimType defaultSimType = CPU;
		const int32_t defaultEmitterCount = 1;

		stream.write(reinterpret_cast<const char *>(&defaultSimType), sizeof(EParticleSimType));
		stream.write(reinterpret_cast<const char *>(&defaultEmitterCount), sizeof(int32_t));
		ParticleEmitter::SerializeInitialData(stream);
	}

	void ParticleSystem::CreateEmitter()
	{
		ParticleEmitter *emitter = new ParticleEmitter(this);
		emitters.push_back(emitter);
	}
}