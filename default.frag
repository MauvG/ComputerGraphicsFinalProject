#version 330 core

out vec4 FragColor;

in vec3 currentPosition;
in vec3 normal;
in vec3 color;
in vec2 textureCoordinate;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

vec4 pointLight()
{	
	float ambient = 0.50f;

	vec3 lightVector = lightPosition - currentPosition;

	float distance = length(lightVector);
	float a = 3.0;
	float b = 0.7;
	float intensity = 1.0f / (a * distance * distance + b * distance + 1.0f);

	vec3 currentNormal = normalize(normal);
	vec3 lightDirection = normalize(lightVector);
	float diffuse = max(dot(currentNormal, lightDirection), 0.0f);

	vec3 viewDirection = normalize(cameraPosition - currentPosition);
	vec3 reflectionDirection = reflect(-lightDirection, currentNormal);

	float specularLight = 0.50f;
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specularAmount * specularLight;

	return (texture(diffuse0, textureCoordinate) * (diffuse * intensity + ambient) + texture(specular0, textureCoordinate).r * specular * intensity) * lightColor;
}


vec4 directLight()
{
	float ambient = 0.50f;

	vec3 currentNormal = normalize(normal);
	vec3 lightDirection = normalize(vec3(1.0f, -1.0f, 1.0f));
	float diffuse = max(dot(currentNormal, lightDirection), 0.0f);

	vec3 viewDirection = normalize(cameraPosition - currentPosition);
	vec3 reflectionDirection = reflect(-lightDirection, currentNormal);

	float specularLight = 0.50f;
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specularAmount * specularLight;

	return (texture(diffuse0, textureCoordinate) * (diffuse + ambient) + texture(specular0, textureCoordinate).r * specular) * lightColor;
}

void main()
{
	FragColor = directLight(); 
}