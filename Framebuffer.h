#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>


class Framebuffer {
public:
    Framebuffer(unsigned int samples, unsigned int gamma, unsigned int width, unsigned int height);
    void Default();
    void Bind(Shader& framebufferShader);
    void Unbind();

private:
    unsigned int samples, gamma, width, height;
    unsigned int rectangleVAO, rectangleVBO;
    unsigned int FBO, RBO;
    unsigned int framebufferTexture, postProcessingFBO, postProcessingTexture;
};

#endif
