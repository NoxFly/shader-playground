/**
 * @author Dorian Thivolle
 */

#pragma once

#include <GL/glew.h>
#include <vector>

struct model {
	GLuint VAO;
	GLuint VBO;
};

model loadModelToVRAM(const std::vector<GLfloat>& data, const GLuint dimension);
model loadModelToVRAM(const std::vector<GLfloat>& data, const std::vector<GLuint>& indices, const GLuint dimension);