#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	GLuint id;
	EBO(std::vector<GLuint>& indices);

	void Bind();
	void Unbind();
	void Delete();
};

#endif