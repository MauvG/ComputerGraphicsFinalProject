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
uniform bool blendTextures;

uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform bool enableFog;

vec4 blendColor()
{
    float heightThreshold = 1.0f;
    vec4 textureOne = texture(diffuse0, textureCoordinate);
    vec4 textureTwo = texture(diffuse1, textureCoordinate);

    if (!blendTextures)
    {
        return textureOne;
    }

    float blendFactor = smoothstep(heightThreshold - 10.0f, heightThreshold + 10.0, height);
    vec4 blendedColor = mix(textureOne, textureTwo, blendFactor);

    return blendedColor;
}

vec4 directLight(vec4 blendedColor)
{
    float ambient = 0.50f;

    vec3 currentNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition);
    float diffuse = max(dot(currentNormal, lightDirection), 0.0f);

    float specular = 0.0f;
    
    if (diffuse != 0.0f)
    {
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 halfwayVector = normalize(viewDirection + lightDirection);
        
        float specularLight = 0.50f;
        float specularAmount = pow(max(dot(currentNormal, halfwayVector), 0.0f), 16);
        specular = specularAmount * specularLight;
    };
    
    float shadow = 0.0f;
    vec3 lightCoordinate = fragPositionLight.xyz / fragPositionLight.w;

    if(lightCoordinate.z <= 1.0f)
    {
        lightCoordinate = (lightCoordinate + 1.0f) / 2.0f;
        float currentDepth = lightCoordinate.z;
        float bias = max(0.025f * (1.0f - dot(currentNormal, lightDirection)), 0.0005f);

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

    return (blendedColor * (diffuse * (1.0f - shadow) + ambient)) * lightColor;
}

void main()
{
    vec4 blendedColor = blendColor();

    vec4 lightColor = directLight(blendedColor);
    float distance = length(currentPosition - cameraPosition);
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

    FragColor = mix(vec4(fogColor, 1.0), lightColor, fogFactor);
    //FragColor = blendedColor;
}
