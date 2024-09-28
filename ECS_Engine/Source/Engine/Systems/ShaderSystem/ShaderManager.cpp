#include "ShaderManager.h"
#include "FileHelper.h"
#include "ShaderProgram.h"
#include "glew/glew.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

namespace LKT
{
	namespace fs = std::filesystem;

	namespace
	{
		static void RemoveExtension(std::string &from)
		{
			fs::path p(from);
			from = p.stem().string();
		}

		static void GetPathEnding(std::string &path)
		{
			fs::path p(path);
			path = p.filename().string();
		}
	}

	ShaderManager &ShaderManager::Get()
	{
		static ShaderManager instance;
		return instance;
	}

	ShaderManager::~ShaderManager()
	{
		for (const auto &it : engineShaders)
		{
			delete it.second;
		}

		engineShaders.clear();
	}

	void ShaderManager::ActivateShader(const std::string &shaderName)
	{
		const auto it = engineShaders.find(shaderName);

		if (it != engineShaders.end())
		{
			if (activeShader != shaderName)
			{
				lastActiveShader = activeShader;
				activeShader = shaderName;
				glUseProgram(engineShaders[shaderName]->GetID());
			}
		}
	}

	void ShaderManager::ActivateLastShader()
	{
		ShaderManager &instance = ShaderManager::Get();
		const auto it = instance.engineShaders.find(instance.lastActiveShader);

		if (it != instance.engineShaders.end())
		{
			glUseProgram(instance.engineShaders[instance.lastActiveShader]->GetID());
		}
	}

	bool ShaderManager::GetUBO(uint32_t uboIndex, uint32_t &outUBO)
	{
		const ShaderManager &instance = ShaderManager::Get();
		const auto it = instance.ubos.find(uboIndex);
		if (it != instance.ubos.end())
		{
			outUBO = it->second;
			return true;
		}
		return false;
	}

	void ShaderManager::SetInt(const std::string &name, int32_t value)
	{
		if (ShaderProgram *sp = GetActiveShader())
		{
			sp->SetInt(name, value);
		}
	}

	void ShaderManager::SetFloat(const std::string &name, float value)
	{
		if (ShaderProgram *sp = GetActiveShader())
		{
			sp->SetFloat(name, value);
		}
	}

	void ShaderManager::SetMat4f(const std::string &name, const glm::mat4 &value)
	{
		if (ShaderProgram *sp = GetActiveShader())
		{
			sp->SetMat4f(name, glm::value_ptr(value));
		}
	}

	uint32_t ShaderManager::GetActiveShaderID() const
	{
		const auto it = engineShaders.find(activeShader);

		if (it != engineShaders.end())
		{
			return it->second->GetID();
		}

		return 0;
	}

	ShaderProgram *ShaderManager::GetActiveShader() const
	{
		const auto it = engineShaders.find(activeShader);

		if (it != engineShaders.end())
		{
			return it->second;
		}

		return nullptr;
	}

	void ShaderManager::LoadEngineShaders()
	{
		/**
		 * This is hardcoded will look for a better way to do it
		 */
		LoadUBOs();

		std::vector<std::string> shaderFiles;

		FileHelper::GetFilesFromDirectory(engineShadersPath, shaderFiles);

		// Compute shaders are single files, so they're in the upper folder section
		for (const std::string &file : shaderFiles)
		{
			std::string noExtensionFile = file;
			LKT::RemoveExtension(noExtensionFile);

			ShaderProgram *sp = new ShaderProgram();
			sp->CompileComputeShader(file, noExtensionFile);

			engineShaders[noExtensionFile] = sp;
		}

		shaderFiles.clear();

		FileHelper::GetDirectoriesFromDirectory(engineShadersPath, shaderFiles);

		for (const std::string &directory : shaderFiles)
		{
			std::vector<std::string> shaders;
			FileHelper::GetFilesFromDirectory(directory, shaders);

			std::string shaderName = directory;
			LKT::GetPathEnding(shaderName);

			std::string vertexShaderPath, fragmentShaderPath;

			for (const std::string &shader : shaders)
			{
				const std::string ext = std::filesystem::path(shader).extension().string();

				if (ext == ".fs")
				{
					fragmentShaderPath = shader;
				}
				else if (ext == ".vs")
				{
					vertexShaderPath = shader;
				}
			}

			if (shaders.size() > 1)
			{
				ShaderProgram *sp = new ShaderProgram();
				sp->CompileShader(vertexShaderPath, fragmentShaderPath, shaderName);
				engineShaders[shaderName] = sp;
			}
		}

		onPostCompile.Invoke();
	}

	void ShaderManager::LoadUBOs()
	{
		uint32_t ubo;

		// Matrices
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
		ubos.emplace(MATRICES_UBO_INDEX, ubo);

		// Lights (TODO)
	}
}