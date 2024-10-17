#version 450

in vec2 TexCoords;
uniform sampler2D textureAtlas;

out vec4 FragColor;

void main()
{
    //FragColor = texture(textureAtlas, TexCoords);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}