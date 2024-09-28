#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "Color.h"
#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace LKT
{
    class ShaderProgram
    {
    public:
        ~ShaderProgram();
        ShaderProgram() = default;

        void CompileShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& inName);
        void CompileComputeShader(const std::string& computeShaderPath, const std::string& inName);

        bool IsValid() const { return id != 0; }
        uint32_t GetID() const { return id; }

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int32_t value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec4(const std::string& name, const Color& value) const;
        void SetMat4f(const std::string& name, const float* value) const;

        void SetVec3Array(const std::string& name, const std::vector<glm::vec3>& data);

    private:
        //Uniform buffers are only set on non compute shaders
        void SetUniformBuffers();
        void CheckCompilerError(uint32_t shader, std::string type, const std::string& shaderPath);

        uint32_t id;
        std::string shaderName;
    };
}
#endif