#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "Color.h"
#include "Math/Numerals.h"
#include "Math/Transforms.h"

#include <string>
#include <vector>

enum class ShaderType
{
    none = 0,
    sprite,
    mesh,
    post_process
};

class ShaderProgram
{
public:
    ~ShaderProgram();
    ShaderProgram() = default;

    void InitializeShader();
    void InitializeShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    bool IsValid() const { return id != 0; }
    uint32 GetID() const { return id; }

    void Use();
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int32 value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const Vector3& value) const;
    void SetVec4(const std::string& name, const Color& value) const;
    void SetMat4f(const std::string& name, const float* value) const;

    void SetVec3Array(const std::string& name, const std::vector<Vector3>& data);   

private:    
    void CheckCompilerError(uint32 shader, std::string type);

    const std::string defaultVertexShaderPath = "Data/DefaultShaders/meshShader.vert";
    const std::string defaultFragmentShaderPath = "Data/DefaultShaders/meshShader.frag";

    uint32 id;
};

#endif