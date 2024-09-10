#version 450 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D screen;

void main() 
{ 
    fragColor = texture(screen, TexCoords); 
}