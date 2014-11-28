#pragma once

#include "Shader.h"

#include <string>

// Handles loading and rendering of model assets. (Raw models without material, textures, transformations)

class Mesh
{
public:
	Mesh(const std::string & filename);
	Mesh() {};

	void setGeometry(GLfloat * vertices, int numVertices);
	void setNormals(GLfloat * normals, int numNormals);
	void setVertexColors(GLfloat * colors, int numColors);
	void setTextureCoordinates(GLfloat * uvs, int numUVs);

    GLfloat * getVertices();
    GLuint getNumVertices();
    GLfloat * getNormals();
    
	void loadFromFile(const std::string & filename);
	void loadVBO();
	void render();

	void destroy();

private:
	GLfloat * m_vertices;		// x, y, z
	GLfloat * m_normals;		// vertex normals
	GLfloat * m_uvs;			// texture coordinates
	GLfloat * m_colors;         // vertex colors
    
    GLuint m_numVertices;
    GLuint m_numNormals;
    GLuint m_numUVs;
    GLuint m_numColors;

	GLuint m_vao;				// vertex array object
	GLuint m_vbo;				// vertex buffer object

	static const GLuint m_vPosition     = 0;    // reference to the variable "vPosition" in the shader
	static const GLuint m_vNormal       = 1;    // reference to the variable "vNormal" in the shader
	static const GLuint m_vTexCoord     = 2;    // reference to the variable "vTexCoord" in the shader
	static const GLuint m_vColor        = 3;    // reference to the variable "vColor" in the shader
};