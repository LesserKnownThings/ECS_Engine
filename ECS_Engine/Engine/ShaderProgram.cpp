#include "ShaderProgram.h"
#include "glew.h"

#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

void ShaderProgram::Use()
{
	glUseProgram(id);
}

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int32>(value));
}

void ShaderProgram::SetInt(const std::string& name, int32 value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::SetVec3(const std::string& name, const Vector3& value) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void ShaderProgram::SetVec4(const std::string& name, const Color& value) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), value.r, value.g, value.b, value.a);
}

void ShaderProgram::SetMat4f(const std::string& name, const float* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, value);
}

void ShaderProgram::SetVec3Array(const std::string& name, const std::vector<Vector3>& data)
{
	int32 uniformLocation = glGetUniformLocation(id, name.c_str());

	if (uniformLocation != -1)
	{
		for (int32 i = 0; i < data.size(); ++i)
		{
			glUniform3fv(uniformLocation + i, 1, glm::value_ptr(data[i]));
		}
	}
}

void ShaderProgram::InitializeShader()
{
    InitializeShader(defaultVertexShaderPath, defaultFragmentShaderPath);
}

void ShaderProgram::InitializeShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
       
        vShaderFile.close();
        fShaderFile.close();
      
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    CheckCompilerError(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    CheckCompilerError(fragment, "FRAGMENT");
    
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    CheckCompilerError(id, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void ShaderProgram::CheckCompilerError(uint32 shader, std::string type)
{
    int32 success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
