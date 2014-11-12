#pragma once

#include "Shader.h"

#include <string>

// Handles loading and rendering of model assets. (Raw models without material, textures, transformations)

class Mesh
{
public:
	Mesh(Shader * shader, const std::string & filename);
	Mesh(Shader * shader);
	Mesh() {};

	void setGeometry(GLfloat * vertices, int numVertices);
	void setNormals(GLfloat * normals, int numNormals);
	void setVertexColors(GLfloat * colors, int numColors);
	void setTextureCoordinates(GLfloat * uvs, int numUVs);

	void loadFromFile(const std::string & filename);
	void loadVBO();
	void render();

	void destroy();

private:
	Shader * m_shader;			// The shader object for this model
    
	GLfloat * m_vertices;		// x, y, z
	GLfloat * m_normals;		// vertex normals
	GLfloat * m_uvs;			// texture coordinates
	GLfloat * m_colors;         // vertex colors
    
	GLuint m_numVertices, m_numNormals, m_numUVs, m_numColors;

	GLuint m_vao;				// vertex array object
	GLuint m_vbo;				// vertex buffer object

	GLuint m_vPosition;         // reference to the variable "vPosition" in the shader
	GLuint m_vNormal;			// reference to the variable "vNormal" in the shader
	GLuint m_vTexCoord;         // reference to the variable "vTexCoord" in the shader
	GLuint m_vColor;			// reference to the variable "vColor" in the shader
};