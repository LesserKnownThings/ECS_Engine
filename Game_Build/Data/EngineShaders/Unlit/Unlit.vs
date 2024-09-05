#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in int boneID;
layout (location = 6) in float weights;

out vec2 TexCoords;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model = mat4(1.0);

void main()
{
    TexCoords = uv;
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}