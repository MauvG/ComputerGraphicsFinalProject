#include "Model.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Model::Model(const std::string& filePath, unsigned int instancing, std::vector<glm::mat4> instanceMatrix) {
    Model::filePath = filePath;
    Model::instancing = instancing;
    Model::instanceMatrix = instanceMatrix;

    LoadModel(filePath);
}

void Model::LoadModel(const std::string& filePath) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    if (!ret) {
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

    for (const auto& anim : model.animations) {
        for (const auto& channel : anim.channels) {
            if (channel.target_path != "translation") continue;

            AnimationChannel animChannel;
            animChannel.nodeIndex = channel.target_node;
            const auto& sampler = anim.samplers[channel.sampler];
            const auto& inputAccessor = model.accessors[sampler.input];
            const auto& outputAccessor = model.accessors[sampler.output];

            std::vector<float> times;
            {
                const auto& bufferView = model.bufferViews[inputAccessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];
                const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + inputAccessor.byteOffset;
                const float* floatData = reinterpret_cast<const float*>(dataPtr);
                times.assign(floatData, floatData + inputAccessor.count);
            }

            std::vector<float> translations;
            {
                const auto& bufferView = model.bufferViews[outputAccessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];
                const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + outputAccessor.byteOffset;
                const float* floatData = reinterpret_cast<const float*>(dataPtr);
                translations.assign(floatData, floatData + outputAccessor.count * 3);
            }

            for (size_t i = 0; i < times.size(); ++i) {
                Keyframe kf;
                kf.time = times[i];
                kf.translation = glm::vec3(
                    translations[i * 3],
                    translations[i * 3 + 1],
                    translations[i * 3 + 2]
                );
                animChannel.keyframes.push_back(kf);
            }

            if (!animChannel.keyframes.empty()) {
                float lastTime = animChannel.keyframes.back().time;
                if (lastTime > animationDuration)
                    animationDuration = lastTime;
            }

            animationChannels.push_back(animChannel);
        }
    }

    std::cout << "Loaded " << animationChannels.size() << " animation channels." << std::endl;
}

void Model::ProcessNode(const tinygltf::Node& node, const glm::mat4& parentTransform) {
    glm::mat4 transform = parentTransform;

    if (!node.matrix.empty()) {
        glm::mat4 matNode = glm::make_mat4(node.matrix.data());
        transform *= matNode;
    }
    else {
        if (!node.translation.empty()) {
            transform = glm::translate(transform, glm::vec3(
                node.translation[0], node.translation[1], node.translation[2]));
        }
        if (!node.rotation.empty()) {
            glm::quat quatNode = glm::quat(
                node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
            transform *= glm::mat4_cast(quatNode);
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
                Vertex vertex;
                vertex.position = glm::vec3(positionData[i * 3], positionData[i * 3 + 1], positionData[i * 3 + 2]);
                vertex.normal = glm::vec3(1.0f); 
                vertex.color = glm::vec3(1.0f);  
                vertex.textureUV = glm::vec2(0.0f);
                vertex.height = 0.0f;
                vertices.push_back(vertex);
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
                if (textureIndex >= 0 && textureIndex < model.textures.size()) {
                    const auto& texture = model.textures[textureIndex];
                    const auto& image = model.images[texture.source];

                    std::string modelDirectory = filePath.substr(0, filePath.find_last_of('/') + 1);
                    std::string texturePath = modelDirectory + image.uri;
                    std::cout << "Loading texture: " << texturePath << std::endl;

                    textures.emplace_back(Texture(texturePath.c_str(), "diffuse", 0));
                }
            }
        }

        meshes.emplace_back(vertices, indices, textures, instancing, instanceMatrix);
        matricesMeshes.emplace_back(transform);
    }
}

std::vector<float> Model::GetAttributeData(const tinygltf::Accessor& accessor) {
    std::vector<float> result;
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

    size_t count = accessor.count;
    size_t stride = 1;
    switch (accessor.type) {
    case TINYGLTF_TYPE_SCALAR: stride = 1; break;
    case TINYGLTF_TYPE_VEC2: stride = 2; break;
    case TINYGLTF_TYPE_VEC3: stride = 3; break;
    case TINYGLTF_TYPE_VEC4: stride = 4; break;
    default: stride = 1; break;
    }

    for (size_t i = 0; i < count * stride; ++i) {
        result.push_back(static_cast<const float*>(reinterpret_cast<const void*>(data))[i]);
    }

    return result;
}

std::vector<GLuint> Model::GetIndices(const tinygltf::Accessor& accessor) {
    std::vector<GLuint> indices;
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

    for (size_t i = 0; i < accessor.count; ++i) {
        switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indices.push_back(static_cast<const GLuint*>(reinterpret_cast<const void*>(data))[i]);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            indices.push_back(static_cast<const GLushort*>(reinterpret_cast<const void*>(data))[i]);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            indices.push_back(static_cast<const GLubyte*>(reinterpret_cast<const void*>(data))[i]);
            break;
        default:
            indices.push_back(0);
            break;
        }
    }

    return indices;
}

void Model::Draw(Shader& shader, Camera& camera, glm::mat4 modelMatrix) {
    for (size_t i = 0; i < meshes.size(); ++i) {
        glm::mat4 finalModel = modelMatrix * matricesMeshes[i];
        meshes[i].Draw(shader, camera, finalModel);
    }
}

void Model::UpdateAnimation(float currentTime) {
    for (auto& channel : animationChannels) {
        if (channel.keyframes.empty()) continue;

        float time = fmod(currentTime, animationDuration);

        Keyframe kf1 = channel.keyframes[0];
        Keyframe kf2 = channel.keyframes[0];
        for (size_t i = 0; i < channel.keyframes.size() - 1; ++i) {
            if (time >= channel.keyframes[i].time && time <= channel.keyframes[i + 1].time) {
                kf1 = channel.keyframes[i];
                kf2 = channel.keyframes[i + 1];
                break;
            }
        }

        float delta = kf2.time - kf1.time;
        float factor = (delta > 0.0f) ? (time - kf1.time) / delta : 0.0f;

        glm::vec3 interpolated = glm::mix(kf1.translation, kf2.translation, factor);

        if (channel.nodeIndex < matricesMeshes.size()) {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matricesMeshes[channel.nodeIndex], scale, rotation, translation, skew, perspective);

            translation = interpolated;

            matricesMeshes[channel.nodeIndex] = glm::translate(glm::mat4(1.0f), translation) *
                glm::mat4_cast(rotation) *
                glm::scale(glm::mat4(1.0f), scale);
        }
    }
}
