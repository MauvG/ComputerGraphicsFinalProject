#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include <fastnoiselite/fast_noise_lite.h>
#include <stb/stb_image.h>
#include <string>

class Terrain {
public:
    Terrain(float size, unsigned int resolution, float heightScale, float noiseFrequency, int octaves, float lacunarity, float gain);
    ~Terrain();
    void Draw(Shader& shader, Camera& camera, glm::mat4 model);

    float GetHeightAt(float x, float z) const;
    float GetSize() const { return size; }

private:
    Mesh* terrainMesh;

    float size;
    unsigned int resolution;
    float heightScale;
    float noiseFrequency;
    int octaves;
    float lacunarity;
    float gain;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    void GenerateTerrain(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

    FastNoiseLite noise;
};

#endif
