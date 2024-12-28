#ifndef SKYBOX_H
#define SKYBOX_H

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>

class Skybox {
public:
    Skybox();
    void Draw(Shader& shader, Camera& camera, int width, int height);

private:
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO, skyboxTexture;
};

#endif
