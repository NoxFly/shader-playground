/**
 * @author Dorian Thivolle
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/type_trait.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include "shader.hpp"
#include "modelLoader.hpp"


struct frustrum {
	float fov;
	float ratio;
	float near;
	float far;
};

union uniformValue {
	GLint i;
	GLfloat f;
	glm::vec2 v2;
	glm::vec3 v3;
	glm::vec4 v4;
	glm::mat2 m2;
	glm::mat3 m3;
	glm::mat4 m4;
};

struct uniform {
	GLint id;
	uniformValue value;
};

struct uniforms {
	uniform
		mvp,
		m,
		v,
		p,
		mouse,
		center,
		resolution,
		time,
		delta,
		ratio,
		zoom;
};

struct MVP {
	glm::mat4 m;
	glm::mat4 v;
	glm::mat4 p;
	glm::mat4 mvp;
};

enum windowMode {
	WINDOWED,
	FULLSCREEN
};

class App {

	public:
		App();
		~App();

		void close();
		void run();
		bool loadFractal(const std::string& name);

	private:
		void init();
		void initGLFW();
		void initGLEW();
		void initSurface();

		bool initShader();

		std::vector<GLfloat> getVerticesScreenSized() const;

		void refreshResolution();
		void refreshSurface();
		void sendUniforms();

		void createWindow();

		void onKey(int key, int scancode, int action, int mods);
		void onMouseMove(double xpos, double ypos);

		void toggleFullscreen();

		windowMode m_windowMode;
		GLFWwindow* m_window;
		GLuint m_windowWidth, m_windowHeight, m_realWidth, m_realHeight;
		frustrum m_frustrum;
		GLuint m_shaderId;
		model m_surface;
		std::string m_fractalName;
		uniforms m_uniforms;
		MVP m_mvp;
		bool m_needEscape;
};