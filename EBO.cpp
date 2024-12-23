#include"EBO.h"

EBO::EBO(std::vector<GLuint>& indices)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &id);
}