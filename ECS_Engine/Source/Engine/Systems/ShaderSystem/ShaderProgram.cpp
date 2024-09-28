#include "ShaderProgram.h"
#include "ShaderManager.h"
#include "glew/glew.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace LKT
{
    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(id);
    }

    void ShaderProgram::SetBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int32_t>(value));
    }

    void ShaderProgram::SetInt(const std::string &name, int32_t value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::SetFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::SetVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
    }

    void ShaderProgram::SetVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
    }

    void ShaderProgram::SetVec4(const std::string &name, const Color &value) const
    {
        glUniform4f(glGetUniformLocation(id, name.c_str()), value.r, value.g, value.b, value.a);
    }

    void ShaderProgram::SetMat4f(const std::string &name, const float *value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, value);
    }

    void ShaderProgram::SetVec3Array(const std::string &name, const std::vector<glm::vec3> &data)
    {
        int32_t uniformLocation = glGetUniformLocation(id, name.c_str());

        if (uniformLocation != -1)
        {
            for (int32_t i = 0; i < data.size(); ++i)
            {
                glUniform3fv(uniformLocation + i, 1, glm::value_ptr(data[i]));
            }
        }
    }

    void ShaderProgram::CompileShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath, const std::string &inName)
    {
        shaderName = inName;
        uint32_t vertex = 0, fragment = 0;

        if (!vertexShaderPath.empty())
        {
            std::string vertexCode;
            std::ifstream vShaderFile;
            std::stringstream vShaderStream;

            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            vShaderFile.open(vertexShaderPath);
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();

            vertexCode = vShaderStream.str();
            const char *vShaderCode = vertexCode.c_str();

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            CheckCompilerError(vertex, "VERTEX", vertexShaderPath);
        }

        if (!fragmentShaderPath.empty())
        {
            std::string fragmentCode;
            std::ifstream fShaderFile;
            std::stringstream fShaderStream;

            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.open(fragmentShaderPath);
            fShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();

            fragmentCode = fShaderStream.str();

            const char *fShaderCode = fragmentCode.c_str();

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            CheckCompilerError(fragment, "FRAGMENT", fragmentShaderPath);
        }

        id = glCreateProgram();

        if (vertex != 0)
            glAttachShader(id, vertex);
        if (fragment != 0)
            glAttachShader(id, fragment);

        glLinkProgram(id);
        CheckCompilerError(id, "PROGRAM", vertexShaderPath);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        SetUniformBuffers();
    }

    void ShaderProgram::CompileComputeShader(const std::string &computeShaderPath, const std::string &inName)
    {
        shaderName = inName;

        std::string computeCode;
        std::ifstream computeFile;

        computeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            computeFile.open(computeShaderPath);
            std::stringstream cShaderStream;

            cShaderStream << computeFile.rdbuf();

            computeFile.close();

            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        const char *cShaderCode = computeCode.c_str();

        unsigned int compute;

        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        CheckCompilerError(compute, "COMPUTE", computeShaderPath);

        id = glCreateProgram();
        glAttachShader(id, compute);
        glLinkProgram(id);
        CheckCompilerError(id, "PROGRAM", computeShaderPath);
    }

    void ShaderProgram::SetUniformBuffers()
    {
        uint32_t matricesUBO = glGetUniformBlockIndex(id, "Matrices");
        glUniformBlockBinding(id, matricesUBO, MATRICES_UBO_INDEX);

        uint32_t lightUBO = glGetUniformBlockIndex(id, "Light");
        glUniformBlockBinding(id, lightUBO, LIGHTS_UBO_INDEX);
    }

    void ShaderProgram::CheckCompilerError(uint32_t shader, std::string type, const std::string &shaderPath)
    {
        int32_t success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << shaderPath << " " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << shaderPath << " " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}