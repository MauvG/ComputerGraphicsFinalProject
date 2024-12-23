#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 cameraMatrix;

void main()
{
	gl_Position = cameraMatrix * model * vec4(aPosition, 1.0f);
}