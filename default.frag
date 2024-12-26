#version 330 core

out vec4 FragColor;

in vec3 currentPosition;
in vec3 normal;
in vec3 color;
in vec2 textureCoordinate;
in vec4 fragPositionLight;
in float height;

uniform sampler2D diffuse0; 
uniform sampler2D specular0;
uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

vec4 directLight()
{
	float ambient = 0.50f;

	vec3 currentNornal = normalize(normal);
	vec3 lightDirection = normalize(lightPosition);
	float diffuse = max(dot(currentNornal, lightDirection), 0.0f);

	float specular = 0.0f;
	
	if (diffuse != 0.0f)
	{
		vec3 viewDirection = normalize(cameraPosition - currentPosition);
		vec3 halfwayVector = normalize(viewDirection + lightDirection);
		
		float specularLight = 0.50f;
		float specularAmount = pow(max(dot(currentNornal, halfwayVector), 0.0f), 16);
		specular = specularAmount * specularLight;
	};
	
	float shadow = 0.0f;
	vec3 lightCoordinate = fragPositionLight.xyz / fragPositionLight.w;

	if(lightCoordinate.z <= 1.0f)
	{
		lightCoordinate = (lightCoordinate + 1.0f) / 2.0f;
		float currentDepth = lightCoordinate.z;
		float bias = max(0.025f * (1.0f - dot(currentNornal, lightDirection)), 0.0005f);

		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);

		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoordinate.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}

		shadow /= pow((sampleRadius * 2 + 1), 2);
	}

	return (texture(diffuse0, textureCoordinate) * (diffuse * (1.0f - shadow) + ambient) + texture(specular0, textureCoordinate).r * specular  * (1.0f - shadow)) * lightColor;
}

void main()
{
	FragColor = directLight();
}