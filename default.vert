#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexture;
layout (location = 3) in vec3 aNormal;


out vec3 color;
out vec2 textureCoordinate;
out vec3 normal;
out vec3 currentPosition;

uniform mat4 cameraMatrix;
uniform mat4 model;


void main()
{
	currentPosition = vec3(model * vec4(aPosition, 1.0f));
	gl_Position = cameraMatrix * vec4(currentPosition, 1.0);

	color = aColor;
	textureCoordinate = aTexture;
	normal = aNormal;
}