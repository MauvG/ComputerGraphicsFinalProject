#ifndef SHADOW_H
#define SHADOW_H

#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>

class Shadows
{
public:
	Shadows(unsigned int shadowMapWidth, unsigned int shadowMapHeight);
	void EnableDepthTest();
	void Bind(Shader& shader);

private:
	unsigned int shadowMapFBO, shadowMap;
	unsigned int shadowMapWidth, shadowMapHeight;

};

#endif
