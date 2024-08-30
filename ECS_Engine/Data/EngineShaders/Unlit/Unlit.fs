#version 330 core
out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffue1;

layout (std140, binding = 1) uniform Light
{
    int val;
};

void main() 
{
    fragColor = texture(texture_diffue1, TexCoords);
}