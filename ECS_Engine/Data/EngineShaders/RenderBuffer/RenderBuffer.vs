#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

out vec2 TexCoords;

void main()
{
    TexCoords = uv;
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
}