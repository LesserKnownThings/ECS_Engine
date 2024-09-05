#pragma once

#include "Delegate.h"
#include "glm/glm.hpp"
#include "ShaderData.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace LKT
{
	DECLARE_DELEGATE(OnPostCompile)

	class ShaderProgram;

	//TODO create a cache for the shader manager so that I don't have to compile all shaders on engine startup
	class ShaderManager
	{
	public:
		static ShaderManager& Get();		

		~ShaderManager();

		void ActivateShader(const std::string& shaderName);

		void SetFloat(const std::string& name, float value);
		void SetMat4f(const std::string& name, const glm::mat4& value);

		uint32_t GetActiveShaderID() const;
		ShaderProgram* GetActiveShader() const;

		//Triggered when the shaders finished compiling
		OnPostCompile onPostCompile;

	private:
		void LoadEngineShaders();

		std::string activeShader;

		const std::string engineShadersPath = "Data/EngineShaders";

		std::unordered_map<std::string, ShaderProgram*> engineShaders;

		friend class Engine;
	};
}