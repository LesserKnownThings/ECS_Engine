#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uv;

layout(std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};

out vec2 TexCoords;

void main()
{
    TexCoords = uv;
    gl_Position = projection * view * vec4(vertex, 1.0);
}