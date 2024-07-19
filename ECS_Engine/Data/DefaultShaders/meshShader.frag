#version 330 core
out vec4 fragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main() 
{
    vec4 diffuseColor = texture(material.texture_diffuse1, TexCoords);
    fragColor = vec4(0, 0, 0, 1);
}