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

	// TODO create a cache for the shader manager so that I don't have to compile all shaders on engine startup
	class ShaderManager
	{
	public:
		static ShaderManager &Get();

		~ShaderManager();

		void ActivateShader(const std::string &shaderName);

		/**
		 * This will activate the last active shader, it will also keep the last active shader as it was
		 * The reaons I'm doing this is because it can only reset the state once,
		 * if I try to set another last active shader it will just go around. An example would be this:
		 * 1. Activate a frame buffer for background rendering and use a custom shader for post processing
		 * 2. Go back to the previous shader so we can render the rest
		 * 3. If we set the previous shader to the new one which is the frame buffer shader
		 * and we still have frame buffers left, it will activate a new shader and then when it will try to set the previous one
		 * it will get stuck in a weird loop where other things won't render because they're using the wrong shader
		 */
		static void ActivateLastShader();

		void SetInt(const std::string &name, int32_t value);
		void SetFloat(const std::string &name, float value);
		void SetMat4f(const std::string &name, const glm::mat4 &value);

		uint32_t GetActiveShaderID() const;
		ShaderProgram *GetActiveShader() const;

		// Triggered when the shaders finished compiling
		OnPostCompile onPostCompile;

	private:
		void LoadEngineShaders();

		std::string activeShader;
		std::string lastActiveShader;

		const std::string engineShadersPath = "Data/EngineShaders";

		std::unordered_map<std::string, ShaderProgram *> engineShaders;

		friend class Engine;
	};
}