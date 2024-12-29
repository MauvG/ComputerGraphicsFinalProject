#include "Mesh.h"

Mesh::Mesh(
	std::vector <Vertex>& vertices,
	std::vector <GLuint>& indices,
	std::vector <Texture>& textures,
	unsigned int instancing,
	std::vector <glm::mat4> instanceMatrix
)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::instancing = instancing;

	vao.Bind();
	VBO vbo(vertices);
	VBO instanceVBO(instanceMatrix);
	EBO ebo(indices);
	
	vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	vao.LinkAttribute(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	vao.LinkAttribute(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	vao.LinkAttribute(vbo, 4, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, height));

	if (instancing != 1)
	{
		instanceVBO.Bind();
		vao.LinkAttribute(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		vao.LinkAttribute(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		vao.LinkAttribute(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		vao.LinkAttribute(instanceVBO, 8, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
	}

	vao.Unbind();
	vbo.Unbind();
	instanceVBO.Unbind();
	ebo.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	shader.Activate();
	vao.Bind();

	unsigned int diffuseNum = 0;
	unsigned int specularNum = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;

		if (type == "diffuse")
		{
			num = std::to_string(diffuseNum++);
		}
		
		textures[i].TextureUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}
	
	glUniform3f(glGetUniformLocation(shader.id, "cameraPosition"), camera.position.x, camera.position.y, camera.position.z);
	camera.Matrix(shader, "cameraMatrix");

	// Check if instance drawing should be performed
	if (instancing == 1)
	{
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 matrixRotation = glm::mat4(1.0f);
		glm::mat4 matrixScale = glm::mat4(1.0f);

		translationMatrix = glm::translate(translationMatrix, translation);
		matrixRotation = glm::mat4_cast(rotation);
		matrixScale = glm::scale(matrixScale, scale);

		glUniformMatrix4fv(glGetUniformLocation(shader.id, "translation"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shader.id, "rotation"), 1, GL_FALSE, glm::value_ptr(matrixRotation));
		glUniformMatrix4fv(glGetUniformLocation(shader.id, "scale"), 1, GL_FALSE, glm::value_ptr(matrixScale));
		glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
	}

	vao.Unbind();
	glActiveTexture(GL_TEXTURE0);

}

void Mesh::UpdateVertices(std::vector<Vertex>& newVertices, std::vector <GLuint>& newIndices)
{
	Mesh::vertices = newVertices;
	Mesh::indices = newIndices;

	vao.Bind();
	VBO vbo(newVertices);
	EBO ebo(newIndices);

	vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	vao.LinkAttribute(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	vao.LinkAttribute(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	vao.LinkAttribute(vbo, 4, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, height));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::UpdateInstanceMatrix(unsigned int newInstancing, std::vector <glm::mat4> newInstanceMatrix)
{
	Mesh::instancing = newInstancing;

	vao.Bind();
	VBO instanceVBO(newInstanceMatrix);
	
	if (instancing != 1)
	{
		instanceVBO.Bind();
		vao.LinkAttribute(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		vao.LinkAttribute(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		vao.LinkAttribute(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		vao.LinkAttribute(instanceVBO, 8, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
	}

	vao.Unbind();
	instanceVBO.Unbind();
}