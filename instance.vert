#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexture;
layout (location = 4) in float aHeight;
layout (location = 5) in mat4 aInstanceMatrix;

out vec3 currentPosition;
out vec3 normal;
out vec3 color;
out vec2 textureCoordinate;
out vec4 fragPositionLight;
out float height;

uniform mat4 cameraMatrix;

uniform mat4 lightProjection;

void main()
{
	currentPosition = vec3(aInstanceMatrix * vec4(aPosition, 1.0f));
	normal = aNormal;
	color = aColor;
	textureCoordinate = aTexture;
	fragPositionLight = lightProjection * vec4(currentPosition, 1.0f);
	height = aHeight;
	
	gl_Position = cameraMatrix * vec4(currentPosition, 1.0);
}