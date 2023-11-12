/**
 * @author Dorian Thivolle
 */

#pragma once

#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

struct shader {
	GLuint id = 0;
	GLuint vertexId = 0;
	GLuint fragmentId = 0;
};

bool loadShader(shader& shader, const std::string& name);
void deleteShader(shader& shader);

bool replaceFragmentShader(shader& shader, const std::string& name);