#include "Shader.h"

GLint Shader::activeProgramID = 0;

Shader::Shader(const std::string & vertexShaderPath, const std::string & fragmentShaderPath) {
	char * vertexCode = readFile(vertexShaderPath);
	char * fragmentCode = readFile(fragmentShaderPath);

	if (!(vertexCode && fragmentCode)) {
		printf("Can't find shader file\n");
		exit(-1);
	}
	GLint vertexID = compileShader(vertexCode, GL_VERTEX_SHADER);
	GLint fragmentID = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

	m_programID = linkShaderProgram(vertexID, fragmentID);
	delete[] vertexCode;
	delete[] fragmentCode;
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

char * Shader::readFile(const std::string & filename) {
	FILE *fp = fopen(filename.c_str(), "r");
    if (fp == nullptr) {
        printf("Shader file not found: %s\n", filename.c_str());
    }

	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	char *contents = new char[file_length+1];
	for (int i = 0; i < file_length+1; i++) {
		contents[i] = 0;
	}
	fread(contents, 1, file_length, fp);
	contents[file_length] = '\0';
	fclose(fp);

	return contents;
}

bool Shader::compiledStatus(GLint shaderID) {
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char *msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete[] msgBuffer;
		return false;
	}
}

GLint Shader::compileShader(const char * shaderSource, GLenum shaderType) {
	GLint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(shaderID);
	bool compiledCorrectly = compiledStatus(shaderID);
	if (compiledCorrectly) {
		return shaderID;
	}
	return -1;
}

bool Shader::linkedStatus(GLint programID) {
	GLint linked = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &linked);
	if (linked) {
		return true;
	}
	else {
		GLint logLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
		char *msgBuffer = new char[logLength];
		glGetProgramInfoLog(programID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete[] msgBuffer;
		return false;
	}
}

GLint Shader::linkShaderProgram(GLint vertexShaderID, GLint fragmentShaderID) {
	GLint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	bool linked = linkedStatus(programID);

	if (linked) {
		return programID;
	}
	return -1;
}

GLint Shader::getUniform(const std::string & name) {
	if (name.empty()) {
		printf("Could not find uniform location: Invalid name.\n");
		exit(-1);
	}

	GLint uniform = glGetUniformLocation(activeProgramID, name.c_str());
	if (uniform == -1) {
		printf("Could not get uniform location with name: %s.\n", name.c_str());
		exit(-1);
	}
	return uniform;
}

void Shader::setUniform(const std::string & name, float x, float y, float z) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform3f(loc, x, y, z);
    }
}

void Shader::setUniform(const std::string & name, const glm::vec3 & v) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform3f(loc, v.x, v.y, v.z);
    }
}

void Shader::setUniform(const std::string & name, const glm::vec4 & v) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
}

void Shader::setUniform(const std::string & name, const glm::mat3 & m) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
    }
}

void Shader::setUniform(const std::string & name, const glm::mat4&  m) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    }
}

void Shader::setUniform(const std::string & name, float val) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform1f(loc, val);
    }
}

void Shader::setUniform(const std::string & name, int val) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
}

void Shader::setUniform(const std::string & name, bool val) {
    GLint loc = getUniform(name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
}

GLint Shader::getProgramID() const {
    return m_programID;
}

void Shader::use() {
	glUseProgram(m_programID);
    activeProgramID = m_programID;
}

void Shader::destroy(void) {
	glDeleteProgram(m_programID);
}
