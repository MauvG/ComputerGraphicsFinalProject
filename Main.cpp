#include "Model.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Framebuffer.h"
#include "Shadows.h"

#include <glm/gtc/matrix_transform.hpp>
#include <random>

const unsigned int width = 1920;
const unsigned int height = 1080;

const unsigned int samples = 2;
const float gamma = 3.0f;

struct InstanceData {
	glm::mat4 modelMatrix;
};

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));

	Model tree("Models/MyTree/scene.gltf");
	glm::mat4 treeModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	//treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(-5.0f, 0.0f, 0.0f));

	Model rock("Models/MyRock/scene.gltf");
	//glm::mat4 rockModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	//rockModelMatrix = glm::translate(rockModelMatrix, glm::vec3(5.0f, 0.0f, 0.0f));

	Model ufo("Models/Ufo/scene.gltf");
	glm::mat4 ufoModel = glm::mat4(1.0f);

	// Terrain
	float terrainSize = 1000.0f;                // Size of the terrain
	unsigned int terrainResolution = 128;       // Resolution (number of vertices per axis)
	float terrainHeightScale = 200.0f;          // Height multiplier (increased for more variation)
	float terrainNoiseFrequency = 0.002f;       // Base frequency for larger features
	int terrainOctaves = 6;                     // Number of noise layers
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
	//terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -5.0f, 0.0f));

	// Number of instances
	const unsigned int numTrees = 2500;
	const unsigned int numRocks = 1000;

	std::vector<InstanceData> treeInstances;
	treeInstances.reserve(numTrees);

	std::vector<InstanceData> rockInstances;
	rockInstances.reserve(numRocks);

	// Random number generators
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> posDist(-terrainSize / 2.0f, terrainSize / 2.0f);
	std::uniform_real_distribution<float> scaleDist(1.0f, 2.0f);
	std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);

	// Generate tree instances
	for (unsigned int i = 0; i < numTrees; ++i)
	{
		float x = posDist(gen);
		float z = posDist(gen);
		float y = terrain.GetHeightAt(x, z) - 0.25f;

		float scale = scaleDist(gen) * 10.0f;
		float rotation = rotDist(gen);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));

		treeInstances.push_back({ model });
	}

	// Generate rock instances
	for (unsigned int i = 0; i < numRocks; ++i)
	{
		float x = posDist(gen);
		float z = posDist(gen);
		float y = terrain.GetHeightAt(x, z) - 0.25f;

		float scale = scaleDist(gen);
		float rotation = rotDist(gen);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));

		rockInstances.push_back({ model });
	}


	// Skybox
	Skybox skybox;

	// Framebuffer
	Framebuffer framebuffer(samples, gamma, width, height);

	// Shadows
	Shadows shadows(8192, 8192);
	
	// Light for shadow
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
	
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

		// Depth testing needed for Shadow Map
		shadows.EnableDepthTest();

		// Draw scene for shadow
		ufo.Draw(shadowMapShader, camera, ufoModel);
		
		// Draw all trees for shadow
		for (const auto& treeInstance : treeInstances)
		{
			tree.Draw(shadowMapShader, camera, treeInstance.modelMatrix);
		}

		// Draw all rocks for shadow
		for (const auto& rockInstance : rockInstances)
		{
			rock.Draw(shadowMapShader, camera, rockInstance.modelMatrix);
		}

		// Switch back to the default
		framebuffer.Default();

		// Handles camera
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);

		// Send the light matrix to the shader
		defaultShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		// Bind the Shadow Map
		shadows.Bind(defaultShader);

		// Draw skybox
		skybox.Draw(skyboxShader, camera, width, height);

		// Draw scene
		terrain.Draw(defaultShader, camera, terrainModel);

		ufo.Draw(defaultShader, camera, ufoModel);

		// Draw all trees
		for (const auto& treeInstance : treeInstances)
		{	
			tree.Draw(defaultShader, camera, treeInstance.modelMatrix);
		}

		// Draw all rocks
		for (const auto& rockInstance : rockInstances)
		{
			rock.Draw(defaultShader, camera, rockInstance.modelMatrix);
		}

		// Bind fbo
		framebuffer.Bind(framebufferShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	defaultShader.Delete();
	skyboxShader.Delete();

	framebufferShader.Delete();
	framebuffer.Unbind();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}