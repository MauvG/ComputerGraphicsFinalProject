#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Standard MVP transform (if you still want the skybox to remain at infinite distance,
    // you could remove the translation from 'view', but you can keep it simple for now).
    gl_Position = projection * view * vec4(aPosition, 1.0);
    TexCoords = aTexCoords;
}
