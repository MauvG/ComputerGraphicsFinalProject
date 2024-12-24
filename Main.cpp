#include"Model.h"

const unsigned int width = 1920;
const unsigned int height = 1080;

unsigned int samples = 8;
float gamma = 1.5f;

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
	-1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.0f   / 3.0f,
	 1.0f,  1.0f, -1.0f,   1.998f / 4.0f, 2.998f / 3.0f,
	 1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.998f / 3.0f,
	-1.0f,  1.0f,  1.0f,   1.002f / 4.0f, 2.0f   / 3.0f,

	// top
	-1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 1.0f   / 3.0f,
	 1.0f, -1.0f,  1.0f,   1.002f / 4.0f, 0.002f / 3.0f,
	 1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 0.002f / 3.0f,
	-1.0f, -1.0f, -1.0f,   1.998f / 4.0f, 1.0f   / 3.0f
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

Vertex groundVertices[] =
{
	// position							  color            normal						uv
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f * 1000, 0.0f * 1000)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f * 1000, 1.0f * 1000)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f * 1000, 1.0f * 1000)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f * 1000, 0.0f * 1000)}
};


GLuint groundIndices[] =
{
	0, 1, 2,
	0, 2, 3
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
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);

	// Shaders
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader framebufferProgram("framebuffer.vert", "framebuffer.frag");
	Shader shadowMapProgram("shadowMap.vert", "shadowMap.frag");

	// Light settings
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);


	// Activate shaders
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.id, "skybox"), 0);

	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.id, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferProgram.id, "gamma"), gamma);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Create the camera object
	Camera camera(width, height, glm::vec3(0.0f, 1.0f, 2.0f));
	
	// Minecraft Tree
	Model minecraftTree("Models/MinecraftTree/scene.gltf");

	// Ground Texture
	Texture groundTextures[]
	{
		Texture("Textures/MinecraftGrassBlockTop.jpg", "diffuse", 0),
		Texture("Textures/MinecraftGrassBlockTop.jpg", "specular", 1)
	};

	// Ground Mesh
	std::vector <Vertex> meshGroundVertices(groundVertices, groundVertices + sizeof(groundVertices) / sizeof(Vertex));
	std::vector <GLuint> meshGroundIndices(groundIndices, groundIndices + sizeof(groundIndices) / sizeof(GLuint));
	std::vector <Texture> meshGroundTextures(groundTextures, groundTextures + sizeof(groundTextures) / sizeof(Texture));
	Mesh ground(meshGroundVertices, meshGroundIndices, meshGroundTextures);
	glm::mat4 groundModel = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1.0f, 1000.0f));

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
		std::cout << "Failed to load cross texture!" << std::endl;
		stbi_image_free(data);
	}

	// Create framebuffer
	unsigned int rectangleVAO, rectangleVBO;
	
	glGenVertexArrays(1, &rectangleVAO);
	glGenBuffers(1, &rectangleVBO);
	glBindVertexArray(rectangleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;


	// Add Shadows
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

	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	// FPS Counter
	double previousTime = 0.0f;
	double currentTime = 0.0f;
	double timeDifference = 0.0f;
	unsigned int counter = 0;



	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Count fps
		currentTime = glfwGetTime();
		timeDifference = currentTime - previousTime;
		counter++;
		
		if (timeDifference > 1.0 / 5.0)
		{
			std::string fps = std::to_string((1.0 / timeDifference) * counter);
			std::string newTitle = "ComputerGraphicsFinalProject - " + fps + " FPS";
			glfwSetWindowTitle(window, newTitle.c_str());
			previousTime = currentTime;
			counter = 0;
		}
			
		
		// Shadows
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(4.0f, 4.0f);

		// Draw scene for shadow map
		minecraftTree.Draw(shadowMapProgram, camera, glm::vec3(0.0f, 0.0f, 0.0f));
		ground.Draw(shadowMapProgram, camera);

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_CULL_FACE);

		// Switch back to the default 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Camera controls
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);
		
		// Shadows
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(glGetUniformLocation(shaderProgram.id, "shadowMap"), 2);

		// Draw skybox
		skyboxShader.Activate();

		glm::mat4 skyboxView = glm::mat4(glm::mat3(glm::lookAt(camera.position, camera.position + camera.orientation, camera.up)));
		glm::mat4 skyboxProjection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);

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
	
		// Draw ground
		ground.Draw(shaderProgram, camera, groundModel);

		// Draw model
		minecraftTree.Draw(shaderProgram, camera, glm::vec3(0.0f, 0.0f, 0.0f));

		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
	
	glDeleteFramebuffers(1, &FBO);
	glDeleteFramebuffers(1, &postProcessingFBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}