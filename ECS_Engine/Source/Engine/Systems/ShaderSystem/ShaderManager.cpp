#include "ShaderManager.h"
#include "FileHelper.h"
#include "ShaderProgram.h"
#include "glew.h"
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
		static void RemoveExtension(std::string& from)
		{
			fs::path p(from);
			from = p.stem().string();		
		}

		static void GetPathEnding(std::string& path)
		{
			fs::path p(path);
			path = p.filename().string();
		}
	}

	ShaderManager& ShaderManager::Get()
	{
		static ShaderManager instance;
		return instance;
	}

	ShaderManager::~ShaderManager()
	{
		for (const auto& it : engineShaders)
		{
			delete it.second;
		}

		engineShaders.clear();
	}

	void ShaderManager::ActivateShader(const std::string& shaderName)
	{
		const auto it = engineShaders.find(shaderName);

		if (it != engineShaders.end())
		{
			if (activeShader != shaderName)
			{
				activeShader = shaderName;
				glUseProgram(engineShaders[shaderName]->GetID());
			}
		}
	}

	void ShaderManager::SetFloat(const std::string& name, float value)
	{
		if (ShaderProgram* sp = GetActiveShader())
		{
			sp->SetFloat(name, value);
		}
	}

	void ShaderManager::SetMat4f(const std::string& name, const glm::mat4& value)
	{
		if (ShaderProgram* sp = GetActiveShader())
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

	ShaderProgram* ShaderManager::GetActiveShader() const
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
		std::vector<std::string> shaderFiles;

		FileHelper::GetFilesFromDirectory(engineShadersPath, shaderFiles);

		//Compute shaders are single files, so they're in the upper folder section
		for (const std::string& file : shaderFiles)
		{
			std::string noExtensionFile = file;
			LKT::RemoveExtension(noExtensionFile);
			
			ShaderProgram* sp = new ShaderProgram();
			sp->CompileComputeShader(file, noExtensionFile);

			engineShaders[noExtensionFile] = sp;
		}

		shaderFiles.clear();

		FileHelper::GetDirectoriesFromDirectory(engineShadersPath, shaderFiles);

		for (const std::string& directory : shaderFiles)
		{
			std::vector<std::string> shaders;
			FileHelper::GetFilesFromDirectory(directory, shaders);

			std::string shaderName = directory;
			LKT::GetPathEnding(shaderName);

			if (shaders.size() > 1)
			{
				ShaderProgram* sp = new ShaderProgram();
				sp->CompileShader(shaders[1], shaders[0], shaderName);
				engineShaders[shaderName] = sp;
			}
		}

		onPostCompile.Invoke();
	}
}