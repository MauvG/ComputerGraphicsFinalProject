#include "Model.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Model::Model(const std::string& filePath) : filePath(filePath) {
    LoadModel(filePath);
}

void Model::LoadModel(const std::string& filePath) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadASCIIFromFile(&model, &err, &warn, filePath)) {
        std::cerr << "Failed to load GLTF model: " << filePath << "\nError: " << err << "\nWarning: " << warn << std::endl;
        return;
    }
    std::cout << "Successfully loaded GLTF model: " << filePath << std::endl;

    if (!model.scenes.empty() && model.defaultScene >= 0) {
        const tinygltf::Scene& scene = model.scenes[model.defaultScene];
        for (int nodeIndex : scene.nodes) {
            ProcessNode(model.nodes[nodeIndex], glm::mat4(1.0f));
        }
    }
}

void Model::ProcessNode(const tinygltf::Node& node, const glm::mat4& parentTransform) {
    glm::mat4 transform = parentTransform;

    if (!node.matrix.empty()) {
        glm::mat4 matNode = glm::mat4(glm::make_mat4(node.matrix.data()));
        transform *= matNode;
    }
    else {
        if (!node.translation.empty()) {
            transform = glm::translate(transform, glm::vec3(
                node.translation[0], node.translation[1], node.translation[2]));
        }
        if (!node.rotation.empty()) {
            transform *= glm::mat4_cast(glm::quat(
                node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]));
        }
        if (!node.scale.empty()) {
            transform = glm::scale(transform, glm::vec3(
                node.scale[0], node.scale[1], node.scale[2]));
        }
    }

    if (node.mesh >= 0) {
        ProcessMesh(model.meshes[node.mesh], transform);
    }

    for (int childIndex : node.children) {
        ProcessNode(model.nodes[childIndex], transform);
    }
}

void Model::ProcessMesh(const tinygltf::Mesh& gltfMesh, const glm::mat4& transform) {
    for (const auto& primitive : gltfMesh.primitives) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
            const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
            const auto& positionData = GetAttributeData(accessor);

            for (size_t i = 0; i < accessor.count; ++i) {
                vertices.push_back(Vertex{
                    glm::vec3(positionData[i * 3], positionData[i * 3 + 1], positionData[i * 3 + 2]),
                    glm::vec3(1.0f),
                    glm::vec3(1.0f),
                    glm::vec2(0.0f) 
                    });
            }
        }

        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const auto& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& texCoordData = GetAttributeData(accessor);

            for (size_t i = 0; i < accessor.count; ++i) {
                vertices[i].textureUV = glm::vec2(
                    texCoordData[i * 2], texCoordData[i * 2 + 1]
                );
            }
        }

        if (primitive.indices >= 0) {
            indices = GetIndices(model.accessors[primitive.indices]);
        }

        std::vector<Texture> textures;
        if (primitive.material >= 0) {
            const auto& material = model.materials[primitive.material];

            if (material.values.find("baseColorTexture") != material.values.end()) {
                int textureIndex = material.values.at("baseColorTexture").TextureIndex();
                if (textureIndex >= 0) {
                    const auto& image = model.images[textureIndex];

                    std::string modelDirectory = filePath.substr(0, filePath.find_last_of('/') + 1);
                    std::string texturePath = modelDirectory + image.uri;
                    std::cout << "Loading texture: " << texturePath << std::endl;

                    textures.push_back(Texture(texturePath.c_str(), "diffuse", 0));
                }
            }
        }

        meshes.emplace_back(vertices, indices, textures);
        matricesMeshes.push_back(transform);
    }
}

std::vector<float> Model::GetAttributeData(const tinygltf::Accessor& accessor) {
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
    const float* floatData = reinterpret_cast<const float*>(data);

    std::vector<float> result(floatData, floatData + accessor.count * 3);
    return result;
}

std::vector<GLuint> Model::GetIndices(const tinygltf::Accessor& accessor) {
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

    std::vector<GLuint> indices;
    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
        const GLuint* intData = reinterpret_cast<const GLuint*>(data);
        indices.insert(indices.end(), intData, intData + accessor.count);
    }
    else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        const GLushort* shortData = reinterpret_cast<const GLushort*>(data);
        indices.insert(indices.end(), shortData, shortData + accessor.count);
    }
    else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        const GLubyte* byteData = reinterpret_cast<const GLubyte*>(data);
        indices.insert(indices.end(), byteData, byteData + accessor.count);
    }
    return indices;
}

void Model::Draw(Shader& shader, Camera& camera, glm::mat4 model) {
    for (size_t i = 0; i < meshes.size(); ++i) {
        shader.Activate();
     
        glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matricesMeshes[i]));
        meshes[i].Draw(shader, camera, model);
    }
}
