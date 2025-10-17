/**
 * @author NoxFly
 */

#include <shader.hpp>

std::string loadShaderFromFile(const char* shaderFilePath) {
    std::string shaderCode;
    std::ifstream shaderFileStream(shaderFilePath, std::ios::in);

    if (shaderFileStream.is_open()) {
        std::stringstream shaderStringStream;
        shaderStringStream << shaderFileStream.rdbuf();
        shaderCode = shaderStringStream.str();
        shaderFileStream.close();
    }
    else {
        std::cerr << "[Shader::loadShaderFromFile] Failed to load shader from file " << shaderFilePath << "\n";
        return "";
    }

    return shaderCode;
}

bool getDependencyContent(const std::string& dependencyPath, std::string& dependencyContent) {
    const std::string depPath = (dependencyPath.front() == '/'
        ? dependencyPath
        : "res/shaders/" + dependencyPath
        ) + ".glsl";

    std::ifstream depFile;

    depFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // do not use try/catch as it is Shader::readAndPrecomputeFile that is treating errors.
    // if it throws here, then the caller will receive that error.

    // open files
    depFile.open(depPath);

    if (!depFile.is_open()) {
        return false;
    }

    std::stringstream depStream;

    depStream << depFile.rdbuf();

    depFile.close();

    dependencyContent = depStream.str();

    return true;
}

bool readAndPrecomputeFile(const std::string& filepath, std::string& shaderContent) {
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        shaderFile.open(filepath);

        if (!shaderFile.is_open()) {
            std::cerr << "[LoadShader] Failed to open file. Maybe it does not exist, or wrong access rights." << std::endl;
            return false;
        }

        std::string lineBuffer;

        // ENHANCEMENT : for scaling, could be defined by rules and splitted and managed by an external entity
        const std::string includeIdentifier = "#include";
        const auto includeIdSize = includeIdentifier.size();

        const std::string versionIdentifier = "#version";

        while (std::getline(shaderFile, lineBuffer)) {
            auto identifierIdx = lineBuffer.find(includeIdentifier);

            if (identifierIdx != lineBuffer.npos) {
                lineBuffer.erase(0, identifierIdx + includeIdSize);
                lineBuffer = trim(lineBuffer);

                // not form of '#include <>' with a character between tags
                if (lineBuffer.size() < 3 || lineBuffer.front() != '<' || lineBuffer.back() != '>') {
                    std::cerr << "[LoadShader] Malformed syntax for include directive." << std::endl;
                }

                const std::string depPath = lineBuffer.substr(1, lineBuffer.size() - 2);
                lineBuffer = "";

                if (!getDependencyContent(depPath, lineBuffer)) {
                    std::cerr << "[LoadShader] Failed to import dependency (" + depPath + ")";
                    shaderFile.close();
                    return false;
                }
            }
            else if ((identifierIdx = lineBuffer.find(versionIdentifier)) != lineBuffer.npos) {
                continue;
            }

            shaderContent += lineBuffer + '\n';
        }

        // close file handlers
        shaderFile.close();

        return true;
    }

    catch (std::ifstream::failure const& e) {
        if (e.code().value() == 1 && shaderFile.is_open()) {
            shaderFile.close();
            return true;
        }
        
#ifdef _DEBUG
        //std::cerr << "[LoadShader] Something went wrong (ifstream failure) : " << e.what() << std::endl;
#endif
        return false;
    }

    catch (std::runtime_error const& e) {
        const std::string what = e.what();

#ifdef _DEBUG
        //std::cerr
        //    << "[LoadShader] Failed to read and parse file "
        //    << filepath  << " : " << what
        //    << std::endl;
#endif
        return false;
    }
}

bool checkCompileErrors(GLuint& shader, const std::string& type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::string msg = infoLog;
            std::cerr << "[Shader::checkCompileErrors] Shader compilation error of type: " << type << "\n" + msg << std::endl;
            return false;
        }
    }

    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (success != GL_TRUE) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::string msg = infoLog;
            std::cerr << "[Shader::checkCompileErrors] Program linking error of type: " << type << "\n" << msg << std::endl;
            return false;
        }
    }

    return true;
}


bool compileShader(GLuint& shader, const std::string& type, const std::string& filepath) {
    GLenum shaderType = type == "VERTEX" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    std::string shaderCode;

    if (type == "VERTEX") {
        shaderCode = R"END(
            #version 460 core

            in vec3 in_Vertex;

            out vec2 fragCoord;

            uniform mat4 MVP;

            void main()
            {
	            fragCoord = vec2(in_Vertex.x, in_Vertex.y);
	            gl_Position = MVP * vec4(in_Vertex, 1.0);
            }
        )END";
    }
    else {
        shaderCode = R"END(
            #version 460 core

            in vec2 fragCoord;

            uniform mat4 MVP;
            uniform mat4 M;
			uniform mat4 V;
			uniform mat4 P;
            uniform vec2 ivMouse;
            uniform vec2 fvCenter;
            uniform vec2 uvResolution;
            uniform float fTime;
            uniform float fDelta;
            uniform float fRatio;
            uniform float fZoom;
            uniform int iIncrement;

            uniform int iMode;

            uniform int vbMousePressed[3];
            uniform int vbKeyPressed[4];
            uniform int vbFlags[10];



            out vec4 fragColor;

            @GLSL

            void main()
            {
	            mainImage();
            }
        )END";

        std::string userCode;
        
        if (!readAndPrecomputeFile("res/shaders/" + filepath + ".frag", userCode)) {
            return false;
        }

        shaderCode = replace(shaderCode, "@GLSL", userCode);
    }

    const GLchar* GLshaderCode = shaderCode.c_str();

    // 2. compile shaders
    shader = glCreateShader(shaderType);

    if (shader == 0) {
        std::cerr << "[Shader::load] Cannot create " << type << " shader" << std::endl;
        return false;
    }

    glShaderSource(shader, 1, &GLshaderCode, NULL);
    glCompileShader(shader);

    if (!checkCompileErrors(shader, type)) {
        glDeleteShader(shader);
        return false;
    }

    return true;
}

void deleteShader(shader& shader) {
    if (glIsProgram(shader.id) == GL_TRUE) {
        glDeleteProgram(shader.id);
        shader.id = -1;
    }

    if (glIsShader(shader.vertexId) == GL_TRUE) {
        glDeleteShader(shader.vertexId);
        shader.vertexId = -1;
    }

    if (glIsShader(shader.fragmentId) == GL_TRUE) {
        glDeleteShader(shader.fragmentId);
        shader.fragmentId = -1;
    }
}

bool loadShader(shader& shader, const std::string& name) {
    // Compile vertex shader and fragment shader
    if (!compileShader(shader.vertexId, "VERTEX", name)) {
        return false;
    }

    if (!compileShader(shader.fragmentId, "FRAGMENT", name)) {
        glDeleteShader(shader.vertexId);
        return false;
    }

    // shader Program
    shader.id = glCreateProgram();
    glAttachShader(shader.id, shader.vertexId);
    glAttachShader(shader.id, shader.fragmentId);

    glLinkProgram(shader.id);

    // delete the shaders as they're linked into our program now and no longer necessary
    //glDeleteShader(shader.vertexId);
    //glDeleteShader(shader.fragmentId);

    if (!checkCompileErrors(shader.id, "PROGRAM")) {
        deleteShader(shader);
        return false;
    }

    return true;
}


bool replaceFragmentShader(shader& shader, const std::string& name) {
    GLuint newFragmentId;

    if (!compileShader(newFragmentId, "FRAGMENT", name)) {
		return false;
	}

	glDetachShader(shader.id, shader.fragmentId);
	glDeleteShader(shader.fragmentId);

	glAttachShader(shader.id, newFragmentId);

	glLinkProgram(shader.id);

    shader.fragmentId = newFragmentId;

    if (!checkCompileErrors(shader.id, "PROGRAM")) {
        deleteShader(shader);
		return false;
	}

	return true;
}