#version 450

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffue1;

void main() 
{
    fragColor = texture(texture_diffue1, TexCoords);
}