#include "Model.h"
#include "Terrain.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <fastnoiselite/fast_noise_lite.h>
#include <random>
#include <iostream>

const unsigned int width = 1920;
const unsigned int height = 1080;

unsigned int samples = 2;
float gamma = 3.0f;

float currentAnimationTime = 0.0f;
float lastFrameTime = 0.0f;

float skyboxVertices[] = {
	// front
	-1.0f,  1.0f,  1.0f,   1.0f / 4.0f,   1.998f / 3.0f,
	 1.0f,  1.0f,  1.0f,   0.0f,          1.998f / 3.0f,
	 1.0f, -1.0f,  1.0f,   0.0f,          1.002f / 3.0f,
	-1.0f, -1.0f,  1.0f,   1.0f / 4.0f,   1.002f / 3.0f,

	// left
	-1.0f,  1.0f, -1.0f,   2.0f / 4.0f,   2.0f / 3.0f,
	-1.0f,  1.0f,  1.0f,   1.0f / 4.0f,   2.0f / 3.0f,
	-1.0f, -1.0f,  1.0f,   1.0f / 4.0f,   1.0f / 3.0f,
	-1.0f, -1.0f, -1.0f,   2.0f / 4.0f,   1.0f / 3.0f,

	// back
	 1.0f,  1.0f, -1.0f,   3.0f / 4.0f,   1.998f / 3.0f,
	-1.0f,  1.0f, -1.0f,   2.0f / 4.0f,   1.998f / 3.0f,
	-1.0f, -1.0f, -1.0f,   2.0f / 4.0f,   1.002f / 3.0f,
	 1.0f, -1.0f, -1.0f,   3.0f / 4.0f,   1.002f / 3.0f,

	 // right
	 1.0f,  1.0f,  1.0f,   1.0f,          1.998f / 3.0f,
	 1.0f,  1.0f, -1.0f,   3.0f / 4.0f,   1.998f / 3.0f,
	 1.0f, -1.0f, -1.0f,   3.0f / 4.0f,   1.002f / 3.0f,
	 1.0f, -1.0f,  1.0f,   1.0f,          1.002f / 3.0f,

	 // bottom
	 -1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.0f / 3.0f,
	  1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.998f / 3.0f,
	  1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.998f / 3.0f,
	 -1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.0f / 3.0f,

	 // top
	 -1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 1.0f / 3.0f,
	  1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 0.002f / 3.0f,
	  1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 0.002f / 3.0f,
	 -1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 1.0f / 3.0f
};

unsigned int skyboxIndices[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// left
	4, 5, 6,
	6, 7, 4,
	// back
	8, 9, 10,
	10, 11, 8,
	// right
	12, 13, 14,
	14, 15, 12,
	// bottom
	16, 17, 18,
	18, 19, 16,
	// top
	20, 21, 22,
	22, 23, 20
};

float rectangleVertices[] =
{
	// Poisiton    // UVs
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "ComputerGraphicsFinalProject", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader framebufferProgram("framebuffer.vert", "framebuffer.frag");
	Shader shadowMapProgram("shadowMap.vert", "shadowMap.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.id, "skybox"), 0);
	
	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.id, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferProgram.id, "gamma"), gamma);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Scene

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));
	
	// Add tree models
	Model tree("Models/MyTree/scene.gltf");
	glm::mat4 treeModel = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

	Model ufo("Models/Ufo/scene.gltf");
	glm::mat4 ufoModel = glm::mat4(1.0f);

	// Terrain
	float terrainSize = 1000.0f;               // 100 units
	unsigned int terrainResolution = 2048;      // 256x256 vertices
	float terrainHeightScale = 1.0f;         // Height multiplier
	float terrainNoiseFrequency = 1.0f;       // Frequency for noise

	// Paths to terrain textures
	std::string terrainDiffusePath = "Textures/GrassDiffuse.jpg";
	std::string terrainSpecularPath = "Textures/GrassSpecular.jpg";

	// Instantiate Terrain
	Terrain terrain(terrainSize, terrainResolution, terrainHeightScale, terrainNoiseFrequency, terrainDiffusePath, terrainSpecularPath);
	glm::mat4 terrainModel = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 200.0f, 1000.0f));

	// Skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
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

	unsigned int skyboxTexture;
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

	// Create Frame Buffer Object
	unsigned int rectangleVAO, rectangleVBO;
	glGenVertexArrays(1, &rectangleVAO);
	glGenBuffers(1, &rectangleVBO);
	glBindVertexArray(rectangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

	// Create Render Buffer Object
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

	unsigned int postProcessingTexture;
	glGenTextures(1, &postProcessingTexture);
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;


	// Shadows
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	unsigned int shadowMapWidth = 8192, shadowMapHeight = 8192;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Light for shadow
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//glm::mat4 orthgonalProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	// Fps counter
	double previousTime = 0.0;
	double currentTime = 0.0;
	double timeDifference;
	unsigned int counter = 0;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		
		// Calculate deltaTime
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Update animation time
		currentAnimationTime += deltaTime;

		// Update UFO animation
		ufo.UpdateAnimation(currentAnimationTime);

		currentTime = glfwGetTime();
		timeDifference = currentTime - previousTime;
		counter++;

		if (timeDifference >= 1.0 / 5.0)
		{
			std::string FPS = std::to_string((1.0 / timeDifference) * counter);
			std::string newTitle = "ComputerGraphicsFinalProject - " + FPS + "FPS";
			glfwSetWindowTitle(window, newTitle.c_str());

			previousTime = currentTime;
			counter = 0;
		}

		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow 			
		ufo.Draw(shadowMapProgram, camera, ufoModel);
		tree.Draw(shadowMapProgram, camera, treeModel);

		// Switch back to the default
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Handles camera
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 10000.0f);

		// Send the light matrix to the shader
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		// Bind the Shadow Map
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(glGetUniformLocation(shaderProgram.id, "shadowMap"), 2);

		// Draw skybox
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

		// Draw the normal scene
		terrain.Draw(shaderProgram, camera, terrainModel);

		ufo.Draw(shaderProgram, camera, ufoModel);
		tree.Draw(shaderProgram, camera, treeModel);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		framebufferProgram.Activate();
		glBindVertexArray(rectangleVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	shaderProgram.Delete();
	skyboxShader.Delete();
	framebufferProgram.Delete();

	glDeleteFramebuffers(1, &FBO);
	glDeleteFramebuffers(1, &postProcessingFBO);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}