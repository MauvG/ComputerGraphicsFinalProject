#include "skybox.h"

float skyboxVertices[] = {
    // Positions          // UVs
    // Front face
    -1.0f,  1.0f,  1.0f,   1.0f / 4.0f,   1.998f / 3.0f,
     1.0f,  1.0f,  1.0f,   0.0f,          1.998f / 3.0f,
     1.0f, -1.0f,  1.0f,   0.0f,          1.002f / 3.0f,
    -1.0f, -1.0f,  1.0f,   1.0f / 4.0f,   1.002f / 3.0f,

    // Left face
    -1.0f,  1.0f, -1.0f,   2.0f / 4.0f,   2.0f / 3.0f,
    -1.0f,  1.0f,  1.0f,   1.0f / 4.0f,   2.0f / 3.0f,
    -1.0f, -1.0f,  1.0f,   1.0f / 4.0f,   1.0f / 3.0f,
    -1.0f, -1.0f, -1.0f,   2.0f / 4.0f,   1.0f / 3.0f,

    // Back face
     1.0f,  1.0f, -1.0f,   3.0f / 4.0f,   1.998f / 3.0f,
    -1.0f,  1.0f, -1.0f,   2.0f / 4.0f,   1.998f / 3.0f,
    -1.0f, -1.0f, -1.0f,   2.0f / 4.0f,   1.002f / 3.0f,
     1.0f, -1.0f, -1.0f,   3.0f / 4.0f,   1.002f / 3.0f,

     // Right face
     1.0f,  1.0f,  1.0f,   1.0f,          1.998f / 3.0f,
     1.0f,  1.0f, -1.0f,   3.0f / 4.0f,   1.998f / 3.0f,
     1.0f, -1.0f, -1.0f,   3.0f / 4.0f,   1.002f / 3.0f,
     1.0f, -1.0f,  1.0f,   1.0f,          1.002f / 3.0f,

     // Bottom face
    -1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.0f / 3.0f,
     1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.998f / 3.0f,
     1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.998f / 3.0f,
    -1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.0f / 3.0f,

    // Top face
   -1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 1.0f / 3.0f,
    1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 0.002f / 3.0f,
    1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 0.002f / 3.0f,
   -1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 1.0f / 3.0f
};

unsigned int skyboxIndices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,
    // Left face
    4, 5, 6,
    6, 7, 4,
    // Back face
    8, 9, 10,
    10, 11, 8,
    // Right face
    12, 13, 14,
    14, 15, 12,
    // Bottom face
    16, 17, 18,
    18, 19, 16,
    // Top face
    20, 21, 22,
    22, 23, 20
};

Skybox::Skybox()
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load("Textures/Skybox.png", &texWidth, &texHeight, &nrChannels, 0);

    if (data)
    {
        stbi_set_flip_vertically_on_load(false);

        GLint format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load skybox texture!" << std::endl;
        stbi_image_free(data);
    }
}

void Skybox::Draw(Shader& skyboxShader, Camera& camera, int width, int height)
{
    skyboxShader.Activate();

    glm::mat4 skyboxView = glm::mat4(glm::mat3(glm::lookAt(camera.position, camera.position + camera.orientation, camera.up)));
    glm::mat4 skyboxProjection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.id, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.id, "projection"), 1, GL_FALSE, glm::value_ptr(skyboxProjection));

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);
    glUniform1i(glGetUniformLocation(skyboxShader.id, "skybox"), 0);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

}
