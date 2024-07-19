#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;

layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in int boneID;
layout (location = 6) in float weights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = texture;
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    //gl_Position = vec4(vertex, 1.0);
}