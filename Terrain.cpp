#include "Terrain.h"
#include <iostream>
#include <algorithm>

Terrain::Terrain(float size, unsigned int resolution, float heightScale, float noiseFrequency, int octaves, float lacunarity, float gain)
    : size(size), resolution(resolution), heightScale(heightScale), noiseFrequency(noiseFrequency), octaves(octaves), lacunarity(lacunarity), gain(gain), terrainMesh(nullptr)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(octaves);
    noise.SetFrequency(noiseFrequency);
    noise.SetFractalLacunarity(lacunarity);
    noise.SetFractalGain(gain);

    GenerateTerrain(vertices, indices);
    CalculateNormals(vertices, indices);

    std::vector<Texture> textures{ Texture("Textures/Grass1.jpg", "diffuse", 0), Texture("Textures/Grass2.jpg", "diffuse", 1) };

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
            float textureScale = 100;
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

void Terrain::Draw(Shader& shader, Camera& camera, glm::mat4 model)
{
    terrainMesh->Draw(shader, camera, model);
}

float Terrain::GetHeightAt(float x, float z) const
{
    float halfSize = size / 2.0f;
    if (x < -halfSize || x > halfSize || z < -halfSize || z > halfSize)
        return 0.0f;

    float step = size / (resolution - 1);
    float localX = x + halfSize;
    float localZ = z + halfSize;
    unsigned int gridX = static_cast<unsigned int>(localX / step);
    unsigned int gridZ = static_cast<unsigned int>(localZ / step);

    gridX = glm::clamp(gridX, 0u, resolution - 2);
    gridZ = glm::clamp(gridZ, 0u, resolution - 2);

    float height00 = vertices[gridZ * resolution + gridX].position.y;
    float height10 = vertices[gridZ * resolution + (gridX + 1)].position.y;
    float height01 = vertices[(gridZ + 1) * resolution + gridX].position.y;
    float height11 = vertices[(gridZ + 1) * resolution + (gridX + 1)].position.y;

    float fracX = (localX - gridX * step) / step;
    float fracZ = (localZ - gridZ * step) / step;

    if (fracX <= (1.0f - fracZ))
    {
        float height = height00 + fracX * (height10 - height00) + fracZ * (height01 - height00);
        return height;
    }
    else
    {
        float height = height11 + (1.0f - fracX) * (height01 - height11) + (1.0f - fracZ) * (height10 - height11);
        return height;
    }
}
