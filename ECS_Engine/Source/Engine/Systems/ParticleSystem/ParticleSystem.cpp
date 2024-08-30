#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Systems/ShaderSystem/ShaderProgram.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/TaskManagerSystem.h"

namespace LKT
{
	ParticleSystem::~ParticleSystem()
	{
		TaskManagerSystem::Get().RemoveTask(this);

		for (ParticleEmitter* emitter : emitters)
		{
			delete emitter;
		}

		emitters.clear();
	}

	ParticleSystem::ParticleSystem()
	{
		TaskManagerSystem::Get().RegisterTask(this, &ParticleSystem::Render, 0, RENDER_HANDLE);
		TaskManagerSystem::Get().RegisterTask(this, &ParticleSystem::Simulate);

		//TODO For testing only, will remove this once emitters can be created from UI
		CreateEmitter();
	}

	void ParticleSystem::Simulate(float deltaTime)
	{
		if (simType == GPU)
		{
			ShaderManager::Get().ActivateShader(computeShaderName);
		}

		for (ParticleEmitter* emitter : emitters)
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

		for (ParticleEmitter* emitter : emitters)
		{
			emitter->Render();
		}
	}

	void ParticleSystem::CreateEmitter()
	{
		ParticleEmitter* emitter = new ParticleEmitter(simType);
		emitters.push_back(emitter);
	}
}