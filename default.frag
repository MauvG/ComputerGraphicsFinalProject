#version 330 core

out vec4 FragColor;

in vec3 currentPosition;
in vec3 normal;
in vec3 color;
in vec2 textureCoordinate;
in vec4 fragPositionLight;
in float height;

uniform sampler2D diffuse0; 
uniform sampler2D diffuse1; 
uniform sampler2D diffuse2; 
uniform sampler2D diffuse3; 

uniform sampler2D specular0;
uniform sampler2D specular1;
uniform sampler2D specular2;
uniform sampler2D specular3;

uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;


vec4 blendTextures()
{
    float grassLimit = 25.0;  // 0-25
    float mudLimit   = 50.0;  // 25-50
    float rockLimit  = 75.0;  // 50-75

    vec4 grassColor = texture(diffuse0, textureCoordinate);
    vec4 mudColor = texture(diffuse1, textureCoordinate);
    vec4 rockColor = texture(diffuse2, textureCoordinate);
    vec4 snowColor = texture(diffuse3, textureCoordinate);

    vec4 blendedColor;

    if (height <= grassLimit)
    {
        blendedColor = grassColor;
    }
    else if (height > grassLimit && height <= mudLimit)
    {
        float blendFactor = (height - grassLimit) / (mudLimit - grassLimit);
        blendedColor = mix(grassColor, mudColor, blendFactor);
    }
    else if (height > mudLimit && height <= rockLimit)
    {
        float blendFactor = (height - mudLimit) / (rockLimit - mudLimit);
        blendedColor = mix(mudColor, rockColor, blendFactor);
    }
    else
    {
        float blendFactor = (height - rockLimit) / (120.0 - rockLimit); // Assume max height is 120
        blendedColor = mix(rockColor, snowColor, blendFactor);
    }

    return blendedColor;
}

vec4 directLight(vec4 blendedColor)
{
    float ambient = 0.50f;

    vec3 currentNormal = normalize(normal);
    vec3 lightDirection = normalize(vec3(1.0f, -1.0f, 1.0f));
    float diffuse = max(dot(currentNormal, lightDirection), 0.0f);

    float specular = 0.0f;
    if (diffuse != 0.0f)
    {
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 halfwayVector = normalize(viewDirection + lightDirection);

        float specularLight = 0.50f;
        float specularAmount = pow(max(dot(currentNormal, halfwayVector), 0.0f), 16);
        specular = specularAmount * specularLight;
    }

    float shadow = 0.0f;
    vec3 lightCoordinates = fragPositionLight.xyz / fragPositionLight.w;

    if (lightCoordinates.z <= 1.0f)
    {
        lightCoordinates = (lightCoordinates + 1.0f) / 2.0f;
        float currentDepth = lightCoordinates.z;
        float bias = 0.005;

        int sampleRadius = 2;
        vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);

        for (int y = -sampleRadius; y <= sampleRadius; y++)
        {
            for (int x = -sampleRadius; x <= sampleRadius; x++)
            {
                float closestDepth = texture(shadowMap, lightCoordinates.xy + vec2(x, y) * pixelSize).r;
                if (currentDepth > closestDepth + bias)
                    shadow += 1.0f;
            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    return (blendedColor * (diffuse * (1.0f - shadow) + ambient) + texture(specular0, textureCoordinate).r * specular * (1.0f - shadow)) * lightColor;
}

void main()
{
	vec4 blendedColor = blendTextures();
	//FragColor = directLight(blendedColor); 
    FragColor = blendedColor; 
}