#include "Terrain.h"
#include <iostream>

Terrain::Terrain(float size, unsigned int resolution, float heightScale, float noiseFrequency, int octaves, float lacunarity, float gain, const std::string& diffuseTexturePath, const std::string& specularTexturePath)
    : size(size), resolution(resolution), heightScale(heightScale), noiseFrequency(noiseFrequency), octaves(octaves), lacunarity(lacunarity), gain(gain), terrainMesh(nullptr)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(octaves);           
    noise.SetFrequency(noiseFrequency);        
    noise.SetFractalLacunarity(lacunarity);     
    noise.SetFractalGain(gain);                

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GenerateTerrain(vertices, indices);
    CalculateNormals(vertices, indices);

    std::vector<Texture> textures = LoadTextures(diffuseTexturePath, specularTexturePath);

    terrainMesh = new Mesh(vertices, indices, textures);
}

Terrain::~Terrain()
{
    if (terrainMesh)
    {
        //terrainMesh->Delete();
        delete terrainMesh;
        terrainMesh = nullptr;
    }
}

void Terrain::GenerateTerrain(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    float step = size / (resolution - 1);
    float halfSize = size / 2.0f;

    for (unsigned int z = 0; z < resolution; ++z)
    {
        for (unsigned int x = 0; x < resolution; ++x)
        {
            Vertex vertex;
            vertex.position.x = -halfSize + x * step;
            vertex.position.z = -halfSize + z * step;
            vertex.position.y = noise.GetNoise(vertex.position.x, vertex.position.z) * heightScale;
            float textureScale = 10000.0f;
            vertex.textureUV = glm::vec2((static_cast<float>(x) / (resolution - 1)) * textureScale, (static_cast<float>(z) / (resolution - 1)) * textureScale);
            vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
            vertex.height = vertex.position.y;
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            vertices.push_back(vertex);
        }
    }

    for (unsigned int z = 0; z < resolution - 1; ++z)
    {
        for (unsigned int x = 0; x < resolution - 1; ++x)
        {
            unsigned int topLeft = z * resolution + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * resolution + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}


void Terrain::CalculateNormals(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    for (auto& vertex : vertices)
    {
        vertex.normal = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        glm::vec3 v0 = vertices[idx0].position;
        glm::vec3 v1 = vertices[idx1].position;
        glm::vec3 v2 = vertices[idx2].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        vertices[idx0].normal += normal;
        vertices[idx1].normal += normal;
        vertices[idx2].normal += normal;
    }

    for (auto& vertex : vertices)
    {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

std::vector<Texture> Terrain::LoadTextures(const std::string& diffusePath, const std::string& specularPath)
{
    std::vector<Texture> textures;

    Texture diffuseTexture(diffusePath.c_str(), "diffuse", 0);
    textures.push_back(diffuseTexture);

    Texture specularTexture(specularPath.c_str(), "specular", 1);
    textures.push_back(specularTexture);

    return textures;
}

void Terrain::Draw(Shader& shader, Camera& camera, glm::mat4 model)
{
    terrainMesh->Draw(shader, camera, model);
}
