#version 330 core

out vec4 FragColor;

in vec2 textureCoordinates;

uniform sampler2D skybox;

void main()
{
    FragColor = texture(skybox, vec2(textureCoordinates.x, 1.0 - textureCoordinates.y));
}
