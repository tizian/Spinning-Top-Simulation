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

	GLint getAttribute(const std::string & name) const;
    
    void setUniform(const std::string & name, float x, float y, float z);
    void setUniform(const std::string & name, const glm::vec3 & v);
    void setUniform(const std::string & name, const glm::vec4 & v);
    void setUniform(const std::string & name, const glm::mat3 & m);
    void setUniform(const std::string & name, const glm::mat4 & m);
    void setUniform(const std::string & name, float val);
    void setUniform(const std::string & name, int val);
    void setUniform(const std::string & name, bool val);
    
    GLint getProgramID() const;

	void use();

	void destroy();

private:
	char * readFile(const std::string & filename);
	bool compiledStatus(GLint shaderID);
	bool linkedStatus(GLint programID);

	GLint compileShader(const char * shaderSource, GLenum shaderType);
	GLint linkShaderProgram(GLint vertexShaderID, GLint fragmentShaderID);
    
    GLint getUniform(const std::string & name) const;
    
    GLint m_programID;
};