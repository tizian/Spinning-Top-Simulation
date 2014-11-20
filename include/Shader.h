#define GLM_FORCE_RADIANS       // TODO: Why is this necessary?!?!

#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string>

class Shader
{
public:
	Shader() {};
	Shader(const std::string & vertexShaderPath, const std::string & fragmentShaderPath);
    
    static GLint getActiveProgramID() { return activeProgramID; }
    
    static void setUniform(const std::string & name, float x, float y, float z);
    static void setUniform(const std::string & name, const glm::vec3 & v);
    static void setUniform(const std::string & name, const glm::vec4 & v);
    static void setUniform(const std::string & name, const glm::mat3 & m);
    static void setUniform(const std::string & name, const glm::mat4 & m);
    static void setUniform(const std::string & name, float val);
    static void setUniform(const std::string & name, int val);
    static void setUniform(const std::string & name, bool val);
    
    GLint getProgramID() const;

	void use();

	void destroy();

private:
    static GLint activeProgramID;
    
	char * readFile(const std::string & filename);
	bool compiledStatus(GLint shaderID);
	bool linkedStatus(GLint programID);

	GLint compileShader(const char * shaderSource, GLenum shaderType);
	GLint linkShaderProgram(GLint vertexShaderID, GLint fragmentShaderID);
    
    static GLint getUniform(const std::string & name);
    
    GLint m_programID;
};