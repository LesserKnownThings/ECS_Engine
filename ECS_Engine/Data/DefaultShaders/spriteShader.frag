#version 330 core
in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D image;
uniform vec4 spriteColor = vec4(1);

void main() 
{
    fragColor = spriteColor * texture(image, TexCoords);
}