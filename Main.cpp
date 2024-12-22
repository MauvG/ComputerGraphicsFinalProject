#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

const unsigned int width = 800;
const unsigned int height = 800;

GLfloat vertices[] =
{
	// Positions          // Color           // UVs

	// Front face
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.25f, 0.33f, // 0
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.50f, 0.33f, // 1
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.50f, 0.66f, // 2
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.25f, 0.66f, // 3

	// Back face
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.75f, 0.33f, // 4
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.99f, 0.33f, // 5
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.99f, 0.66f, // 6
	-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  0.75f, 0.66f, // 7

	// Left face
	-0.5f, -0.5f, -0.5f,  0.2f, 0.3f, 0.4f,  0.01f, 0.33f, // 8
	-0.5f, -0.5f,  0.5f,  0.3f, 0.4f, 0.5f,  0.25f, 0.33f, // 9
	-0.5f,  0.5f,  0.5f,  0.4f, 0.5f, 0.6f,  0.25f, 0.66f, // 10
	-0.5f,  0.5f, -0.5f,  0.5f, 0.6f, 0.7f,  0.01f, 0.66f, // 11

	// Right face
	 0.5f, -0.5f, -0.5f,  0.6f, 0.7f, 0.8f,  0.50f, 0.33f, // 12
	 0.5f, -0.5f,  0.5f,  0.7f, 0.8f, 0.9f,  0.75f, 0.33f, // 13
	 0.5f,  0.5f,  0.5f,  0.8f, 0.9f, 1.0f,  0.75f, 0.66f, // 14
	 0.5f,  0.5f, -0.5f,  0.9f, 1.0f, 0.1f,  0.50f, 0.66f, // 15

	// Up face
	-0.5f,  0.5f, -0.5f,  0.1f, 0.2f, 0.3f,  0.25f, 0.66f, // 16
	 0.5f,  0.5f, -0.5f,  0.2f, 0.3f, 0.4f,  0.50f, 0.66f, // 17
	 0.5f,  0.5f,  0.5f,  0.3f, 0.4f, 0.5f,  0.50f, 0.99f, // 18
	-0.5f,  0.5f,  0.5f,  0.4f, 0.5f, 0.6f,  0.25f, 0.99f, // 19

	// Down face
	-0.5f, -0.5f, -0.5f,  0.5f, 0.4f, 0.3f,  0.25f, 0.01f, // 20
	 0.5f, -0.5f, -0.5f,  0.6f, 0.5f, 0.4f,  0.50f, 0.01f, // 21
	 0.5f, -0.5f,  0.5f,  0.7f, 0.6f, 0.5f,  0.50f, 0.33f, // 22
	-0.5f, -0.5f,  0.5f,  0.8f, 0.7f, 0.6f,  0.25f, 0.33f  // 23
};

GLuint indices[] =
{
	// Front face
	0, 1, 2,
	0, 2, 3,

	// Back face
	4, 5, 6,
	4, 6, 7,

	// Left face
	8, 9,10,
	8,10,11,

	// Right face
	12,13,14,
	12,14,15,

	// Up face
	16,17,18,
	16,18,19,

	// Down face
	20,21,22,
	20,22,23
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

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttribute(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	Texture brickTex("MinecraftGrassBlock.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.TextureUnit(shaderProgram, "texture0", 0);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Activate();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		int modelLoc = glGetUniformLocation(shaderProgram.id, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		int viewLoc = glGetUniformLocation(shaderProgram.id, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		
		int projLoc = glGetUniformLocation(shaderProgram.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		brickTex.Bind();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	
	brickTex.Delete();
	shaderProgram.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}
