/**
 * @author NoxFly
 */

#include <App.hpp>

static void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}

App::App() :
	m_windowMode(windowMode::WINDOWED),
	m_window(nullptr),
	m_windowWidth(1280),
	m_windowHeight(720),
	m_realWidth(m_windowWidth),
	m_realHeight(m_windowHeight),
	m_frustrum{ 90.f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.f },
	m_shader{},
	m_surface{ 0, 0 },
	m_fractalName("loop"),
	m_uniforms{},
	m_mvp{},
	m_fps{},
	m_zooming(0),
	m_displacement(0, 0),
	m_vsync(true),
	m_needEscape(false),
	m_mouseFlagsUniforms{},
	m_boolFlagsUniforms{},
	m_keySpecialFlagsUniforms{},
	m_keyTabUniform(0),
	m_mouseFragLoc(-1),
	m_keysFragLoc(-1),
	m_flagsFragLoc(-1),
	m_keyTabFragLoc(-1)
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

	for (unsigned int i = 0; i < MOUSE_BTN_COUNT; i++) {
		m_mouseFlagsUniforms[i] = GL_FALSE;
	}
	
	for (unsigned int i = 0; i < KEY_FLAGS_COUNT; i++) {
		m_boolFlagsUniforms[i] = GL_FALSE;
	}

	for (unsigned int i = 0; i < KEY_SPECIAL_COUNT; i++) {
		m_keySpecialFlagsUniforms[i] = GL_FALSE;
	}
}

void App::close() {
	if (m_surface.VAO > 0) {
		glDeleteVertexArrays(1, &m_surface.VAO);
	}

	if (m_surface.VBO > 0) {
		glDeleteBuffers(1, &m_surface.VBO);
	}

	deleteShader(m_shader);

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
	reset();

	m_fps.currentTime = (float)glfwGetTime();
	m_fps.lastFrame = 0;

	m_uniforms.delta.value.f = 0;
	m_uniforms.zoom.value.f = 1.0f;
	m_uniforms.center.value.v2 = glm::vec2(0.0f, 0.0f);

	glfwShowWindow(m_window);

	while (!glfwWindowShouldClose(m_window) && !m_needEscape)
	{
		// update
		updateFPS();


		if (m_zooming != 0) {
			m_uniforms.zoom.value.f *= std::pow(1.02f, m_zooming);
		}

		if (m_displacement.x != 0) {
			m_uniforms.center.value.v2.x += m_displacement.x * 0.01f / m_uniforms.zoom.value.f;
		}

		if (m_displacement.y != 0) {
			m_uniforms.center.value.v2.y += m_displacement.y * 0.01f / m_uniforms.zoom.value.f;
		}

		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glUseProgram(m_shader.id);
		glBindVertexArray(m_surface.VAO);

		sendUniforms();

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		// This is for debug purpose only
		// it is spamming "1282" error code in certain cases
		// because not uniforms are used in the shader
		/*GLenum err = glGetError();

		if (err != GL_NO_ERROR) {
			std::cerr << "Error: " << err << std::endl;
		}*/
	}

	if (m_needEscape) {
		glfwHideWindow(m_window);
	}
	else {
		close();
		init();
	}
}

void App::updateFPS() {
	// Time update
	m_fps.currentTime = (float)glfwGetTime();
	m_uniforms.time.value.f = m_fps.currentTime;

	// delta update
	m_uniforms.delta.value.f = m_fps.currentTime - m_fps.lastTime;

	// nbFrame counter update
	m_fps.nbFrames++;

	if (m_uniforms.delta.value.f >= 1.0) { // If last cout was more than 1 sec ago
		const double fps = 1.0 / m_uniforms.delta.value.f;

		std::stringstream ss;
		ss << "ShaderPlayground" << " [" << (m_fps.nbFrames-1) << " FPS]";

		glfwSetWindowTitle(m_window, ss.str().c_str());

		// reset counter
		m_fps.nbFrames = 0;

		// lastTime update
		m_fps.lastTime  = m_fps.currentTime;
	}
}

void App::reset() {
	m_uniforms.zoom.value.f = 1.0f;
	m_uniforms.center.value.v2 = glm::vec2(0.0f, 0.0f);
	m_uniforms.increment.value.i = 0;
	glfwSetTime(0);
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
	glUniform1i(m_uniforms.increment.id, m_uniforms.increment.value.i);
	glUniform1i(m_keyTabFragLoc, m_keyTabUniform);
	glUniform1iv(m_mouseFragLoc, MOUSE_BTN_COUNT, (GLint*)m_mouseFlagsUniforms);
	glUniform1iv(m_keysFragLoc, KEY_SPECIAL_COUNT, (GLint*)m_keySpecialFlagsUniforms);
	glUniform1iv(m_flagsFragLoc, KEY_FLAGS_COUNT, (GLint*)m_boolFlagsUniforms);

	/*std::cout << "flags: "
		<< m_boolFlagsUniforms[0]
		<< m_boolFlagsUniforms[1]
		<< m_boolFlagsUniforms[2]
		<< m_boolFlagsUniforms[3]
		<< m_boolFlagsUniforms[4]
		<< m_boolFlagsUniforms[5]
		<< m_boolFlagsUniforms[6]
		<< m_boolFlagsUniforms[7]
		<< m_boolFlagsUniforms[8]
		<< m_boolFlagsUniforms[9]
		<< std::endl;

	std::cout << "keys: "
		<< m_keySpecialFlagsUniforms[0]
		<< m_keySpecialFlagsUniforms[1]
		<< m_keySpecialFlagsUniforms[2]
		<< m_keySpecialFlagsUniforms[3]
		<< std::endl;

	std::cout << std::endl;*/
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

	// --- set event callbacks ---

	// keyboard & mouse pos input callback
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onKey(key, scancode, action, mods);
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onMouseMove(xpos, ypos);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onMouseButton(button, action, mods);
	});

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		app->onWindowResize(width, height);
	});
}


void App::onKey(int key, int scancode, int action, int mods) {
	if (action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_I:
				m_uniforms.increment.value.i++;
				break;
			case GLFW_KEY_D:
				m_uniforms.increment.value.i--;
				break;
		}
	}

	if (action == GLFW_PRESS) {
		switch (key) {
			case  GLFW_KEY_ESCAPE:
				m_needEscape = true;
				break;
			case GLFW_KEY_RIGHT_SHIFT:
				m_keySpecialFlagsUniforms[0] = GL_TRUE;
				break;
			case GLFW_KEY_RIGHT_CONTROL:
				m_keySpecialFlagsUniforms[1] = GL_TRUE;
				break;
			case GLFW_KEY_LEFT_ALT:
				m_keySpecialFlagsUniforms[2] = GL_TRUE;
				break;
			case GLFW_KEY_SPACE:
				m_keySpecialFlagsUniforms[3] = GL_TRUE;
				break;
			// ---
			// Left Shift & Ctrl are for zooming
			// Right Shift & Ctrl are for free uniforms
			// Use of "+=" to cancel zoom if both are pressed.
			case GLFW_KEY_LEFT_SHIFT:
				m_zooming = 1;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				m_zooming = -1;
				break;
			// ---
			case GLFW_KEY_LEFT:
				m_displacement.x = -1;
				break;
			case GLFW_KEY_RIGHT:
				m_displacement.x = 1;
				break;
			case GLFW_KEY_DOWN:
				m_displacement.y = -1;
				break;
			case GLFW_KEY_UP:
				m_displacement.y = 1;
				break;
			case GLFW_KEY_I:
				m_uniforms.increment.value.i++;
				break;
			case GLFW_KEY_D:
				m_uniforms.increment.value.i--;
				break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_F5:
				refreshShader();
				break;
			case GLFW_KEY_F8:
				toggleVSync();
				break;
			case GLFW_KEY_F9:
				reset();
				break;
			case GLFW_KEY_F11:
				toggleFullscreen();
				break;
			case GLFW_KEY_0:
			case GLFW_KEY_1:
			case GLFW_KEY_2:
			case GLFW_KEY_3:
			case GLFW_KEY_4:
			case GLFW_KEY_5:
			case GLFW_KEY_6:
			case GLFW_KEY_7:
			case GLFW_KEY_8:
			case GLFW_KEY_9:
				m_boolFlagsUniforms[key - GLFW_KEY_0] = m_boolFlagsUniforms[key - GLFW_KEY_0] == GL_TRUE
					? GL_FALSE
					: GL_TRUE;
				break;
			case GLFW_KEY_RIGHT_SHIFT:
				m_keySpecialFlagsUniforms[0] = GL_FALSE;
				break;
			case GLFW_KEY_RIGHT_CONTROL:
				m_keySpecialFlagsUniforms[1] = GL_FALSE;
				break;
			case GLFW_KEY_LEFT_ALT:
				m_keySpecialFlagsUniforms[2] = GL_FALSE;
				break;
			case GLFW_KEY_SPACE:
				m_keySpecialFlagsUniforms[3] = GL_FALSE;
				break;
			case GLFW_KEY_TAB:
				m_keyTabUniform = (m_keyTabUniform + 1) % 3;
				break;
			case GLFW_KEY_LEFT_SHIFT:
			case GLFW_KEY_LEFT_CONTROL:
				m_zooming = 0;
				break;
			case GLFW_KEY_UP:
			case GLFW_KEY_DOWN:
				m_displacement.y = 0;
				break;
			case GLFW_KEY_LEFT:
			case GLFW_KEY_RIGHT:
				m_displacement.x = 0;
				break;
		}
	}
}

void App::onMouseButton(int button, int action, int mods) {
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			m_mouseFlagsUniforms[0] = action == GLFW_PRESS;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			m_mouseFlagsUniforms[1] = action == GLFW_PRESS;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_mouseFlagsUniforms[2] = action == GLFW_PRESS;
			break;
	}
}


void App::onMouseMove(double xpos, double ypos) {
	m_uniforms.mouse.value.v2.x = (float)xpos;
	m_uniforms.mouse.value.v2.y = (float)ypos;
}

void App::onWindowResize(int width, int height) {
	refreshResolution();
	refreshSurface();
}

void App::toggleFullscreen() {
	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

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

	onWindowResize(0, 0);
}

void App::toggleVSync() {
	m_vsync = !m_vsync;

	if (m_vsync) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
}


void App::initGLFW() {
	if (!glfwInit())
	{
		std::cerr << "glfwInit failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Hint
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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

	glfwSwapInterval(1);
}


bool App::initShader() {
	deleteShader(m_shader); // destroy previous shader if exists

	if (!loadShader(m_shader, m_fractalName)) {
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

	m_uniforms.center.value.v2	= glm::vec2(0.0f, 0.0f);

	// retrieve layout (location = ?) for UNIFORMS
	m_uniforms.mvp.id			= glGetUniformLocation(m_shader.id, "MVP");
	m_uniforms.m.id				= glGetUniformLocation(m_shader.id, "M");
	m_uniforms.v.id				= glGetUniformLocation(m_shader.id, "V");
	m_uniforms.p.id				= glGetUniformLocation(m_shader.id, "P");
	m_uniforms.mouse.id			= glGetUniformLocation(m_shader.id, "ivMouse");
	m_uniforms.center.id		= glGetUniformLocation(m_shader.id, "fvCenter");
	m_uniforms.resolution.id	= glGetUniformLocation(m_shader.id, "uvResolution");
	m_uniforms.time.id			= glGetUniformLocation(m_shader.id, "fTime");
	m_uniforms.delta.id			= glGetUniformLocation(m_shader.id, "fDelta");
	m_uniforms.ratio.id			= glGetUniformLocation(m_shader.id, "fRatio");
	m_uniforms.zoom.id			= glGetUniformLocation(m_shader.id, "fZoom");
	m_uniforms.increment.id		= glGetUniformLocation(m_shader.id, "iIncrement");
	m_mouseFragLoc				= glGetUniformLocation(m_shader.id, "vbMousePressed");
	m_keysFragLoc				= glGetUniformLocation(m_shader.id, "vbKeyPressed");
	m_flagsFragLoc				= glGetUniformLocation(m_shader.id, "vbFlags");
	m_keyTabFragLoc				= glGetUniformLocation(m_shader.id, "iMode");

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

void App::refreshShader() {
	if (!replaceFragmentShader(m_shader, m_fractalName)) {
		std::cerr << "Error: failed to reload shader." << std::endl;
	}
}