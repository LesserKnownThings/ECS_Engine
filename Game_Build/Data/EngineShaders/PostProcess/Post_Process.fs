#version 450

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D image;

void main() { fragColor = texture(image, TexCoords); }