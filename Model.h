#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tinygltf/tiny_gltf.h>
#include "Mesh.h"

// Keyframe structure for translation
struct Keyframe {
    float time;
    glm::vec3 translation;
};

// Animation channel structure
struct AnimationChannel {
    int nodeIndex;
    std::vector<Keyframe> keyframes;
};

class Model {
public:
    Model(const std::string& filePath);
    void Draw(Shader& shader, Camera& camera, glm::mat4 model);

    // New function to update animations
    void UpdateAnimation(float currentTime);

private:
    tinygltf::Model model;
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> matricesMeshes;
    std::string filePath;

    // Animation data
    std::vector<AnimationChannel> animationChannels;
    float animationDuration = 0.0f;

    void LoadModel(const std::string& filePath);
    void ProcessNode(const tinygltf::Node& node, const glm::mat4& parentTransform);
    void ProcessMesh(const tinygltf::Mesh& gltfMesh, const glm::mat4& transform);

    std::vector<float> GetAttributeData(const tinygltf::Accessor& accessor);
    std::vector<GLuint> GetIndices(const tinygltf::Accessor& accessor);
};

#endif
