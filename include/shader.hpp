/**
 * @author Dorian Thivolle
 */

#pragma once

#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

bool loadShader(GLuint& shaderId, const std::string& name);
void deleteShader(GLuint programId);