#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexture;

out vec3 currentPosition;
out vec3 normal;
out vec3 color;
out vec2 textureCoordinate;

uniform mat4 cameraMatrix;
uniform mat4 model;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	currentPosition  = vec3(model * translation * rotation * scale * vec4(aPosition, 1.0f));
	normal = aNormal;
	color = aColor;
	textureCoordinate = mat2(0.0, -1.0, 1.0, 0.0) * aTexture;

	gl_Position = cameraMatrix * vec4(currentPosition, 1.0);
}