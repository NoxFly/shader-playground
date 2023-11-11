/**
 * @author Dorian Thivolle
 */

#include <App.hpp>

static void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}

App::App():
	m_windowMode(windowMode::WINDOWED),
	m_window(nullptr),
	m_windowWidth(1280),
	m_windowHeight(720),
	m_realWidth(m_windowWidth),
	m_realHeight(m_windowHeight),
	m_frustrum{ 90.f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.f },
	m_shaderId(-1),
	m_surface{ 0, 0 },
	m_fractalName("loop"),
	m_uniforms{},
	m_mvp{},
	m_needEscape(false)
{
	glfwSetErrorCallback(error_callback);
	init();
}

App::~App() {
	close();
}

void App::init() {
	initGLFW();
	createWindow();
	refreshResolution();
	initGLEW();
	initSurface();
}

void App::close() {
	if (m_surface.VAO > 0) {
		glDeleteVertexArrays(1, &m_surface.VAO);
	}

	if (m_surface.VBO > 0) {
		glDeleteBuffers(1, &m_surface.VBO);
	}

	if (m_shaderId > -1) {
		glDeleteProgram(m_shaderId);
	}

	if (m_window != nullptr) {
		glfwDestroyWindow(m_window);
	}

	glfwTerminate();
}

bool App::loadFractal(const std::string& name) {
	m_fractalName = name;
	return initShader();
}

void App::run() {
	m_needEscape = false;

	refreshResolution();

	float currentFrame = (float)glfwGetTime();
	float lastFrame = 0;

	m_uniforms.delta.value.f = 0;
	m_uniforms.zoom.value.f = 1.0f;
	m_uniforms.center.value.v2 = glm::vec2(0.0f, 0.0f);

	glfwShowWindow(m_window);

	while (!glfwWindowShouldClose(m_window) && !m_needEscape)
	{
		// update
		m_uniforms.time.value.f = (float)glfwGetTime();
		currentFrame = m_uniforms.time.value.f * 1000;
		m_uniforms.delta.value.f = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//m_uniforms.zoom.value.f -= 0.001f;

		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glUseProgram(m_shaderId);
		glBindVertexArray(m_surface.VAO);

		sendUniforms();

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		GLenum err = glGetError();

		if (err != GL_NO_ERROR) {
			std::cerr << "Error: " << err << std::endl;
		}
	}

	if (m_needEscape) {
		glfwHideWindow(m_window);
	}
	else {
		close();
		init();
	}
}

void App::refreshResolution() {
	int w, h;

	glfwGetWindowSize(m_window, &w, &h);

	m_realWidth = w;
	m_realHeight = h;

	glViewport(0, 0, w, h);

	const int halfWinWidth = w / 2;
	const int halfWinHeight = h / 2;

	const glm::vec3 eye(halfWinWidth, halfWinHeight, 350);
	const glm::vec3 target(halfWinWidth, halfWinHeight, 0);
	const glm::vec3 up(0, 1, 0);

	m_frustrum.ratio = (float)w / (float)h;

	m_uniforms.m.value.m4 = glm::mat4(1);
	m_uniforms.v.value.m4 = glm::lookAt(eye, target, up);
	m_uniforms.p.value.m4 = glm::perspective(glm::radians(m_frustrum.fov), m_frustrum.ratio, m_frustrum.near, m_frustrum.far);

	m_uniforms.mvp.value.m4 = m_uniforms.p.value.m4 * m_uniforms.v.value.m4 * m_uniforms.m.value.m4;
	m_uniforms.resolution.value.v2 = glm::vec2(w, h);
	m_uniforms.ratio.value.f = m_frustrum.ratio;
}

void App::sendUniforms() {
	glUniformMatrix4fv(m_uniforms.mvp.id, 1, GL_FALSE, glm::value_ptr(m_uniforms.mvp.value.m4));
	glUniformMatrix4fv(m_uniforms.m.id, 1, GL_FALSE, glm::value_ptr(m_uniforms.m.value.m4));
	glUniformMatrix4fv(m_uniforms.v.id, 1, GL_FALSE, glm::value_ptr(m_uniforms.v.value.m4));
	glUniformMatrix4fv(m_uniforms.p.id, 1, GL_FALSE, glm::value_ptr(m_uniforms.p.value.m4));
	glUniform2fv(m_uniforms.center.id, 1, glm::value_ptr(m_uniforms.center.value.v2));
	glUniform2fv(m_uniforms.mouse.id, 1, glm::value_ptr(m_uniforms.mouse.value.v2));
	glUniform2fv(m_uniforms.resolution.id, 1, glm::value_ptr(m_uniforms.resolution.value.v2));
	glUniform1f(m_uniforms.time.id, m_uniforms.time.value.f);
	glUniform1f(m_uniforms.delta.id, m_uniforms.delta.value.f);
	glUniform1f(m_uniforms.ratio.id, m_uniforms.ratio.value.f);
	glUniform1f(m_uniforms.zoom.id, m_uniforms.zoom.value.f);
}

void App::createWindow() {
	// --- create window ---
	m_window = glfwCreateWindow(m_realWidth, m_realHeight, "ShaderPlayground", NULL, NULL);

	if (!m_window)
	{
		std::cerr << "glfwCreateWindow failed" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_window, videoMode->width / 2 - m_realWidth / 2, videoMode->height / 2 - m_realHeight / 2);

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);
}


void App::onKey(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case  GLFW_KEY_ESCAPE:
				m_needEscape = true;
				break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_F5:
				initShader();
				break;
			case GLFW_KEY_F11:
				toggleFullscreen();
				break;
		}
	}
}

void App::onMouseMove(double xpos, double ypos) {
	m_uniforms.mouse.value.v2.x = (float)xpos;
	m_uniforms.mouse.value.v2.y = (float)ypos;
}

void App::toggleFullscreen() {
	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	GLFW_DONT_CARE;

	// windowed -> fullscreen
	if (m_windowMode == windowMode::WINDOWED) {
		glfwSetWindowMonitor(m_window, nullptr, 0, 0, videoMode->width, videoMode->height, GLFW_DONT_CARE);
		glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_FALSE);
		m_windowMode = windowMode::FULLSCREEN;
	}
	// fullscreen -> windowed
	else {
		glfwSetWindowMonitor(m_window, nullptr, videoMode->width / 2 - m_windowWidth / 2, videoMode->height / 2 - m_windowHeight / 2, m_windowWidth, m_windowHeight, GLFW_DONT_CARE);
		glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_TRUE);
		m_windowMode = windowMode::WINDOWED;
	}

	refreshResolution();
	refreshSurface();
}


void App::initGLFW() {
	if (!glfwInit())
	{
		std::cerr << "glfwInit failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Hint
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}


void App::initGLEW() {
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		std::cerr << "Glew Init Error: " << glewGetErrorString(err) << std::endl;
		exit(EXIT_FAILURE);
	}

	//Depth Buffer activation
	glEnable(GL_DEPTH_TEST);
	// anti-aliasing
	glEnable(GL_MULTISAMPLE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// keyboard & mouse pos input callback
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onKey(key, scancode, action, mods);
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onMouseMove(xpos, ypos);
	});

	glfwSwapInterval(1);
}


bool App::initShader() {
	deleteShader(m_shaderId); // destroy previous shader if exists

	if (!loadShader(m_shaderId, m_fractalName)) {
		return false;
	}

	m_uniforms.mvp				= { -1, 0 };
	m_uniforms.m				= { -1, 0 };
	m_uniforms.v				= { -1, 0 };
	m_uniforms.p				= { -1, 0 };
	m_uniforms.mouse			= { -1, 0 };
	m_uniforms.center			= { -1, 0 };
	m_uniforms.resolution		= { -1, 0 };
	m_uniforms.time				= { -1, 0 };
	m_uniforms.delta			= { -1, 0 };
	m_uniforms.ratio			= { -1, 0 };
	m_uniforms.zoom				= { -1, 0 };

	// retrieve layout (location = ?) for UNIFORMS
	m_uniforms.mvp.id			= glGetUniformLocation(m_shaderId, "MVP");
	m_uniforms.m.id				= glGetUniformLocation(m_shaderId, "M");
	m_uniforms.v.id				= glGetUniformLocation(m_shaderId, "V");
	m_uniforms.p.id				= glGetUniformLocation(m_shaderId, "P");
	m_uniforms.mouse.id			= glGetUniformLocation(m_shaderId, "ivMouse");
	m_uniforms.center.id		= glGetUniformLocation(m_shaderId, "fvCenter");
	m_uniforms.resolution.id	= glGetUniformLocation(m_shaderId, "uvResolution");
	m_uniforms.time.id			= glGetUniformLocation(m_shaderId, "fTime");
	m_uniforms.delta.id			= glGetUniformLocation(m_shaderId, "fDelta");
	m_uniforms.ratio.id			= glGetUniformLocation(m_shaderId, "fRatio");
	m_uniforms.zoom.id			= glGetUniformLocation(m_shaderId, "fZoom");

	return true;
}

std::vector<GLfloat> App::getVerticesScreenSized() const {
	int w, h;

	glfwGetWindowSize(m_window, &w, &h);

	const float top = 0;
	const float left = 0;
	const float bottom = (float)h;
	const float right = (float)w;

	const std::vector<GLfloat> vertices{
		left,	top,
		right,	top,
		right,  bottom,
		left,	top,
		left,	bottom,
		right,  bottom
	};

	return vertices;
}

void App::initSurface() {
	m_surface = loadModelToVRAM(getVerticesScreenSized(), 2);
}

void App::refreshSurface() {
	if (m_surface.VAO > 0) {
		glDeleteVertexArrays(1, &m_surface.VAO);
	}

	if (m_surface.VBO > 0) {
		glDeleteBuffers(1, &m_surface.VBO);
	}

	initSurface();
}