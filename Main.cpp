#include "Model.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Framebuffer.h"
#include "Shadows.h"

#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <set>

const unsigned int width = 1920;
const unsigned int height = 1080;

const unsigned int samples = 2;
const float gamma = 3.0f;

int main()
{
	// Setup
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

	// Shaders
	Shader defaultShader("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader framebufferShader("framebuffer.vert", "framebuffer.frag");
	Shader shadowMapShader("shadowMap.vert", "shadowMap.frag");
	Shader instanceShader("instance.vert", "default.frag");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);

	defaultShader.Activate();
	glUniform4f(glGetUniformLocation(defaultShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(defaultShader.id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.id, "skybox"), 0);

	framebufferShader.Activate();
	glUniform1i(glGetUniformLocation(framebufferShader.id, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferShader.id, "gamma"), gamma);

	instanceShader.Activate();
	glUniform4f(glGetUniformLocation(instanceShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(instanceShader.id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Fog
	glm::vec3 fogColor(0.0, 0.3, 0.5);
	float fogStart = 50.0f;
	float fogEnd = 100.0f; 

	defaultShader.Activate();
	glUniform3f(glGetUniformLocation(defaultShader.id, "fogColor"), fogColor.x, fogColor.y, fogColor.z);
	glUniform1f(glGetUniformLocation(defaultShader.id, "fogStart"), fogStart);
	glUniform1f(glGetUniformLocation(defaultShader.id, "fogEnd"), fogEnd);

	instanceShader.Activate();
	glUniform3f(glGetUniformLocation(instanceShader.id, "fogColor"), fogColor.x, fogColor.y, fogColor.z);
	glUniform1f(glGetUniformLocation(instanceShader.id, "fogStart"), fogStart);
	glUniform1f(glGetUniformLocation(instanceShader.id, "fogEnd"), fogEnd);

	// Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));

	Model ufo("Models/Ufo/scene.gltf");
	glm::mat4 ufoModel = glm::mat4(1.0f);

	// Terrain movement
	float terrainOffsetX = 0.0;
	float terrainOffsetZ = 0.0;
	float moveSpeed = 10.0f;

	// Terrain
	float terrainSize = fogEnd + 100;                // Size of the terrain
	unsigned int terrainResolution = 256;       // Resolution (number of vertices per axis)
	float terrainHeightScale = 20.0f;          // Height multiplier (adjust as needed)
	float terrainNoiseFrequency = 0.02f;        // Base frequency for larger features
	int terrainOctaves = 4;                     // Number of noise layers
	float terrainLacunarity = 2.0f;             // Frequency multiplier per octave
	float terrainGain = 0.5f;                   // Amplitude multiplier per octave

	Terrain terrain(
		terrainSize,
		terrainResolution,
		terrainHeightScale,
		terrainNoiseFrequency,
		terrainOctaves,
		terrainLacunarity,
		terrainGain
	);

	glm::mat4 terrainModel = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// Trees
	float treeNoise = 5000.0f;
	float treeScale = 0.5f;
	std::vector<glm::mat4> treeInstances = terrain.GenerateObjectPositions(3.0f, treeNoise, treeScale, terrainOffsetX, terrainOffsetZ);
	Model tree("Models/MyTree/scene.gltf", treeInstances.size(), treeInstances);

	// Rocks
	//float rockNoise = 300.0f;
	//float rockScale = 0.01f;
	//std::vector<glm::mat4> rockInstances = terrain.GenerateObjectPositions(3.0f, treeNoise, rockScale, terrainOffsetX, terrainOffsetZ);
	//Model rock("Models/MyRock/scene.gltf", rockInstances.size(), rockInstances);

	// Skybox
	Skybox skybox;

	// Framebuffer
	Framebuffer framebuffer(samples, gamma, width, height);

	// Shadows
	Shadows shadows(8192, 8192);
	
	// Light for shadow
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//glm::mat4 orthgonalProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
	
	defaultShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(defaultShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	instanceShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(instanceShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	// Animation
	float currentAnimationTime = 0.0f;
	float lastFrameTime = 0.0f;

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

		// Count fps
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

		// Move terrain
		bool terrainMoved = false;
		glm::vec3 cameraForward = glm::normalize(camera.GetForward());
		glm::vec3 cameraRight = glm::normalize(glm::cross(cameraForward, glm::vec3(0.0f, 1.0f, 0.0f)));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			terrainOffsetX += cameraForward.x * moveSpeed * deltaTime;
			terrainOffsetZ += cameraForward.z * moveSpeed * deltaTime;
			terrainMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			terrainOffsetX -= cameraForward.x * moveSpeed * deltaTime;
			terrainOffsetZ -= cameraForward.z * moveSpeed * deltaTime;
			terrainMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			terrainOffsetX -= cameraRight.x * moveSpeed * deltaTime;
			terrainOffsetZ -= cameraRight.z * moveSpeed * deltaTime;
			terrainMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			terrainOffsetX += cameraRight.x * moveSpeed * deltaTime;
			terrainOffsetZ += cameraRight.z * moveSpeed * deltaTime;
			terrainMoved = true;
		}

		if (terrainMoved)
		{
			terrain.UpdateTerrain(terrainOffsetX, terrainOffsetZ);
			terrainModel = glm::mat4(1.0f);

			if (timeDifference > 1.0 / 10.0)
			{

				// Generate positions for trees
				treeInstances = terrain.GenerateObjectPositions(3, treeNoise, treeScale, terrainOffsetX, terrainOffsetZ);
				tree.UpdateInstances(static_cast<unsigned int>(treeInstances.size()), treeInstances);

				// Generate positions for rocks
				//rockInstances = terrain.GenerateObjectPositions(3, rockNoise, rockScale, terrainOffsetX, terrainOffsetZ);
				//rock.UpdateInstances(static_cast<unsigned int>(rockInstances.size()), rockInstances);

				//std::cout << "Trees: " << treeInstances.size() << " Rocks: " << rockInstances.size() << std::endl;
			}
		}

		// Depth testing needed for Shadow Map
		shadows.EnableDepthTest();

		// Draw scene for shadow
		shadowMapShader.Activate();
		ufo.Draw(shadowMapShader, camera, ufoModel);
	
		//// Draw all trees and rocks for shadow using instancing
		tree.Draw(shadowMapShader, camera);
		//rock.Draw(shadowMapShader, camera);

		// Switch back to the default
		framebuffer.Default();

		// Handles camera
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, fogEnd + 100);

		// Send the light matrix to the shader
		defaultShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		// Bind the Shadow Map
		shadows.Bind(defaultShader);

		instanceShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		shadows.Bind(instanceShader);

		// Draw skybox
		skybox.Draw(skyboxShader, camera, width, height);

		// Draw scene		
		glEnable(GL_CULL_FACE);

		defaultShader.Activate();
		terrain.Draw(defaultShader, camera, terrainModel);
		ufo.Draw(defaultShader, camera, ufoModel);

		// Draw all trees and rocks using instancing
		instanceShader.Activate();
		tree.Draw(instanceShader, camera);
		//rock.Draw(instanceShader, camera);

		glDisable(GL_CULL_FACE);

		// Bind fbo
		framebuffer.Bind(framebufferShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	defaultShader.Delete();
	skyboxShader.Delete();
	framebufferShader.Delete();
	shadowMapShader.Delete();
	instanceShader.Delete();

	framebuffer.Unbind();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}