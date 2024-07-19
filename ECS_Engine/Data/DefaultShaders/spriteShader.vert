#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 cam;

void main()
{
    TexCoords = texture;
    gl_Position = cam * model * vec4(vertex, 1.0);
    //gl_Position = model * vec4(vertex, 1.0);
}