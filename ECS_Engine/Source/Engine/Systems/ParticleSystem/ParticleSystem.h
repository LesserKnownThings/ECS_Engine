#pragma once

#include "ParticleSimType.h"

#include <string>
#include <vector>

namespace LKT
{
	class ParticleEmitter;
	class ShaderProgram;

	class ParticleSystem
	{
	public:
		~ParticleSystem();
		ParticleSystem();

		void Simulate(float deltaTime);
		void Render();		

	private:
		std::vector<ParticleEmitter*> emitters;

		void CreateEmitter();

		EParticleSimType simType = GPU;

		std::string shaderNameGPU = "Billboard_Unlit_GPU";
		std::string shaderNameCPU = "Billboard_Unlit_CPU";
		std::string computeShaderName = "Particles";
	};
}