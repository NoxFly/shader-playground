/**
 * @author Dorian Thivolle
 */

#include "modelLoader.hpp"

#include <iostream>

model loadModelToVRAM(
	const std::vector<GLfloat>& data,
	const GLuint dimension
)
{
	const size_t fsize = sizeof(float);
	const size_t dataSize = data.size() * fsize;

	const unsigned int lineSize = dimension * fsize;
	const unsigned int vertexOffset = 0;


	// gen VAO, VBO
	// lock these, and transfer model data to VRAM

	GLuint VAO, VBO;

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	if (VAO == 0 || VBO == 0) {
		std::cerr << "Failed to generate vao/vbo" << std::endl;
		return { 0, 0 };
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);

	// lock layout locations so the program knows which data corresponds to what

	// position attribute
	// glVertexAttribPoint(layoutLocation, bufferCount, float, flase, combien pour faire une ligne, le décalage dans la ligne)
	glVertexAttribPointer(0, dimension, GL_FLOAT, GL_FALSE, lineSize, (void*)vertexOffset);
	glEnableVertexAttribArray(0); // layout location 0

	// unlock VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return { VAO, VBO };
}

model loadModelToVRAM(
	const std::vector<GLfloat>& data,
	const std::vector<GLuint>& indices,
	const GLuint dimension
)
{
	const size_t fsize = sizeof(float);
	const size_t dataSize = data.size() * fsize;

	const unsigned int lineSize = dimension * fsize;
	const unsigned int vertexOffset = 0;


	// gen VAO, VBO
	// lock these, and transfer model data to VRAM

	GLuint VAO, VBO, EBO;

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	if (VAO == 0 || VBO == 0 || EBO == 0) {
		std::cerr << "Failed to generate vao/vbo/ebo" << std::endl;
		return { 0, 0 };
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);

	// lock layout locations so the program knows which data corresponds to what

	// position attribute
	// glVertexAttribPoint(layoutLocation, bufferCount, float, flase, combien pour faire une ligne, le décalage dans la ligne)
	glVertexAttribPointer(0, dimension, GL_FLOAT, GL_FALSE, lineSize, (void*)vertexOffset);
	glEnableVertexAttribArray(0); // layout location 0

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// unlock VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return { VAO, VBO };
}