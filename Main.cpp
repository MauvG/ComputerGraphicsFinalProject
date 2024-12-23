#include"Model.h"

const unsigned int width = 800;
const unsigned int height = 800;

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

	Shader shaderProgram("default.vert", "default.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	Model model("Models/MinecraftTree/scene.gltf");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

		model.Draw(shaderProgram, camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}