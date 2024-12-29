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
	float fogStart = 25.0f;
	float fogEnd = 50.0f; 
	float cameraEnd = fogEnd + 50.0f;

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

	// Terrain
	float terrainSize = cameraEnd;              // Size of the terrain
	unsigned int terrainResolution = 512;       // Resolution (number of vertices per axis)
	float terrainHeightScale = 20.0f;           // Height multiplier (adjust as needed)
	float terrainNoiseFrequency = 0.02f;        // Base frequency for larger features
	int terrainOctaves = 4;						// Number of noise layers
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

	// Terrain variables
	glm::mat4 terrainModel = glm::mat4(1.0f);
	float terrainOffsetX = 0.0;
	float terrainOffsetZ = 0.0;
	float generationThreshold = terrainSize * 0.25;
	
	terrain.UpdateTerrain(terrainOffsetX, terrainOffsetZ);
	terrainModel = glm::mat4(1.0f);

	// Trees
	float treeNoise = 5000.0f;
	float treeScale = 0.5f; 
	std::vector<glm::mat4> treeInstances = terrain.GenerateObjectPositions(3.0f, treeNoise, treeScale, terrainOffsetX, terrainOffsetZ, 1.25f);
	Model tree("Models/MyTree/scene.gltf", treeInstances.size(), treeInstances);

	// Ufos
	//float ufoNoise = 10.0f;
	//float ufoScale = 0.5f;
	//std::vector<glm::mat4> ufoInstances = terrain.GenerateObjectPositions(3.0f, ufoNoise, ufoScale, terrainOffsetX, terrainOffsetZ, 5.0f);
	//Model ufo("Models/Ufo/scene.gltf", ufoInstances.size(), ufoInstances);
	
	Model ufo1("Models/MyUfo/scene.gltf");
	glm::mat4 ufo1ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 5.0f, 30.0f));

	Model ufo2("Models/MyUfo/scene.gltf");
	glm::mat4 ufo2ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 10.0f, -45.0f));

	Model ufo3("Models/MyUfo/scene.gltf");
	glm::mat4 ufo3ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 7.5f, -10.0f));


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

	float distanceTravelledX = 0.0f;
	float distanceTravelledZ = 0.0f;
	glm::vec3 previousPosition = camera.position;

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
		ufo1.UpdateAnimation(currentAnimationTime);
		ufo2.UpdateAnimation(currentAnimationTime);
		ufo3.UpdateAnimation(currentAnimationTime);

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

		distanceTravelledX = camera.position.x - previousPosition.x;
		distanceTravelledZ = camera.position.z - previousPosition.z;
		//std::cout << distanceTravelledX << ", " << distanceTravelledZ << std::endl;

		if (abs(distanceTravelledX) > generationThreshold || abs(distanceTravelledZ) > generationThreshold)
		{
			terrainOffsetX += distanceTravelledX;
			terrainOffsetZ += distanceTravelledZ;

			camera.ResetPosition();

			terrain.UpdateTerrain(terrainOffsetX, terrainOffsetZ);
			terrainModel = glm::mat4(1.0f);

			// Generate positions for trees
			treeInstances = terrain.GenerateObjectPositions(3, treeNoise, treeScale, terrainOffsetX, terrainOffsetZ, 1.25f);
			tree.UpdateInstances(static_cast<unsigned int>(treeInstances.size()), treeInstances);

			//ufoInstances = terrain.GenerateObjectPositions(3, ufoNoise, ufoScale, terrainOffsetX, terrainOffsetZ, 5.0f);
			//ufo.UpdateInstances(static_cast<unsigned int>(ufoInstances.size()), ufoInstances);

			// Generate positions for rocks
			//rockInstances = terrain.GenerateObjectPositions(3, rockNoise, rockScale, terrainOffsetX, terrainOffsetZ);
			//rock.UpdateInstances(static_cast<unsigned int>(rockInstances.size()), rockInstances);

			//std::cout << "Trees: " << treeInstances.size() << " Rocks: " << rockInstances.size() << std::endl;

			ufo1ModelMatrix = glm::translate(ufo1ModelMatrix, glm::vec3(-distanceTravelledX, 0.0f, -distanceTravelledZ));
			ufo2ModelMatrix = glm::translate(ufo2ModelMatrix, glm::vec3(-distanceTravelledX, 0.0f, -distanceTravelledZ));
			ufo3ModelMatrix = glm::translate(ufo3ModelMatrix, glm::vec3(-distanceTravelledX, 0.0f, -distanceTravelledZ));

			distanceTravelledX = 0.0f;
			previousPosition = camera.position;
		}

		// Depth testing needed for Shadow Map
		shadows.EnableDepthTest();

		glEnable(GL_CULL_FACE);

		// Draw scene for shadow using instancing
		shadowMapShader.Activate();
		
		tree.Draw(shadowMapShader, camera);
		ufo1.Draw(shadowMapShader, camera, ufo1ModelMatrix);
		ufo2.Draw(shadowMapShader, camera, ufo2ModelMatrix);
		ufo3.Draw(shadowMapShader, camera, ufo3ModelMatrix);

		//rock.Draw(shadowMapShader, camera);

		glDisable(GL_CULL_FACE);

		// Switch back to the default
		framebuffer.Default();

		// Handles camera
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, cameraEnd);

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
		ufo1.Draw(defaultShader, camera, ufo1ModelMatrix);
		ufo2.Draw(defaultShader, camera, ufo2ModelMatrix);
		ufo3.Draw(defaultShader, camera, ufo3ModelMatrix);

		// Draw instances
		instanceShader.Activate();
		tree.Draw(instanceShader, camera);
		//ufo.Draw(instanceShader, camera);
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