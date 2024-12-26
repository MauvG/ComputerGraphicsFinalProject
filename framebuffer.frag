#version 330 core

out vec4 FragColor;
in vec2 textureCoordinates;

uniform sampler2D screenTexture;
uniform float gamma;

void main()
{
    vec4 fragment = texture(screenTexture, textureCoordinates);
    FragColor.rgb = pow(fragment.rgb, vec3(1.0f / gamma));
}