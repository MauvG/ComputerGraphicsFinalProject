#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Mesh.h" 
#include <fastnoiselite/fast_noise_lite.h>
#include <stb/stb_image.h>
#include <string>

class Terrain {
public:
    Terrain(float size, unsigned int resolution, float heightScale, float noiseFrequency, const std::string& diffuseTexturePath, const std::string& specularTexturePath);
    ~Terrain();

    void Draw(Shader& shader, Camera& camera, glm::mat4 model);

private:
    Mesh* terrainMesh;

    float size;
    unsigned int resolution;
    float heightScale;
    float noiseFrequency;

    void GenerateTerrain(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    std::vector<Texture> LoadTextures(const std::string& diffusePath, const std::string& specularPath);

    FastNoiseLite noise;
};

#endif
