#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D skybox;

void main()
{
    FragColor = texture(skybox, vec2(TexCoords.x, 1.0 - TexCoords.y));
}
