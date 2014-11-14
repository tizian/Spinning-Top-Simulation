#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Mesh::Mesh(const std::string & filename) {
	glGenVertexArrays(1, &m_vao);		// Generate a VAO
	glGenBuffers(1, &m_vbo);			// Generate a VBO
	m_vertices = m_normals = m_uvs = m_colors = NULL;
	m_numVertices = m_numNormals = m_numUVs = m_numColors = 0;

	loadFromFile(filename);
}

void Mesh::destroy() {
	glDisableVertexAttribArray(m_vPosition);
	glDisableVertexAttribArray(m_vNormal);
	glDisableVertexAttribArray(m_vTexCoord);
	glDisableVertexAttribArray(m_vColor);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void Mesh::setGeometry(GLfloat *vertices, int numVertices) {
	this->m_vertices = vertices;
	this->m_numVertices = numVertices;
}

void Mesh::setNormals(GLfloat *normals, int numNormals) {
	this->m_normals = normals;
	this->m_numNormals = numNormals;
}

void Mesh::setTextureCoordinates(GLfloat *uvs, int numUVs) {
	this->m_uvs = uvs;
	this->m_numUVs = numUVs;
}

void Mesh::setVertexColors(GLfloat *colors, int numColors) {
    this->m_colors = colors;
    this->m_numColors = numColors;
}

void Mesh::loadVBO() {
	// Bind the vao
	glBindVertexArray(m_vao);

	// Bind the vbo as the current VBO.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	int offset = 0;

	// Calculate the size of the buffer we need
	int sizeBuffer = (3*m_numVertices + 3*m_numNormals + 2*m_numUVs + 4*m_numColors) * sizeof(GLfloat);

	// Call glBufferData and tell the GPU how big the buffer is. We don't load the data yet.
	glBufferData(GL_ARRAY_BUFFER, sizeBuffer, NULL, GL_STATIC_DRAW);

	// If the vertices aren't NULL, load them onto the GPU. Offset is currently 0.
	if (m_vertices) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, m_numVertices*sizeof(GLfloat), this->m_vertices);
		glVertexAttribPointer(m_vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
		offset += m_numVertices*3*sizeof(GLfloat);
	}
	// Load in the vertex normals right after the vertex coordinates.
	if (m_normals) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, m_numNormals*sizeof(GLfloat), this->m_normals);
		glVertexAttribPointer(m_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += m_numNormals*3*sizeof(GLfloat);
	}
	// Load in the texture coordinates right after the normals.
	if (m_uvs) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, m_numUVs*sizeof(GLfloat), this->m_uvs);
		glVertexAttribPointer(m_vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += m_numUVs*2*sizeof(GLfloat);
	}
	// Load in the color coordinates right after the texture coordinates.
	if (m_colors) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, m_numColors*sizeof(GLfloat), this->m_colors);
		glVertexAttribPointer(m_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		// offset += numColors*4*sizeof(GLfloat);
	}
}

void Mesh::render() {
	glBindVertexArray(m_vao);

	if (m_vertices) {
		glEnableVertexAttribArray(m_vPosition);
	}
	if (m_normals) {
		glEnableVertexAttribArray(m_vNormal);
	}
	if (m_uvs) {
		glEnableVertexAttribArray(m_vTexCoord);
	}
	if (m_colors) {
		glEnableVertexAttribArray(m_vColor);
	}

	// Actual draw
	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

	if (m_vertices) {
		glDisableVertexAttribArray(m_vPosition);
	}
	if (m_normals) {
		glDisableVertexAttribArray(m_vNormal);
	}
	if (m_uvs) {
		glDisableVertexAttribArray(m_vTexCoord);
	}
	if (m_colors) {
		glDisableVertexAttribArray(m_vColor);
	}
}

void Mesh::loadFromFile(const std::string & filename) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast);
	if (!scene) {
		printf("ERROR: reading mesh %s\n", filename.c_str());
		exit(-1);
	}

	aiMesh *mesh = scene->mMeshes[0];

	float *vertexArray;
	float *normalArray;
	float *uvArray = new float[0];

	int numTriangles = mesh->mNumFaces*3;

	int numUvCoords = mesh->GetNumUVChannels();

	vertexArray = new float[numTriangles*3];
	normalArray = new float[numTriangles*3];
	if (numUvCoords > 0) {
		uvArray = new float[numTriangles*2];
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace &face = mesh->mFaces[i];

		for (int j = 0; j < 3; j++) {
			if (numUvCoords > 0) {
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
				memcpy(uvArray, &uv, sizeof(float)*2);
				uvArray += 2;
			}

			aiVector3D normal = mesh->mNormals[face.mIndices[j]];
			memcpy(normalArray, &normal, sizeof(float)*3);
			normalArray += 3;

			aiVector3D pos = mesh->mVertices[face.mIndices[j]];
			memcpy(vertexArray, &pos, sizeof(float)*3);
			vertexArray += 3;
		}
	}

	if (numUvCoords > 0) {
		uvArray -= numTriangles*2;
	}
	normalArray -= numTriangles*3;
	vertexArray -= numTriangles*3;

	setGeometry(vertexArray, numTriangles*3);
	setNormals(normalArray, numTriangles*3);
	if (numUvCoords > 0) {
		setTextureCoordinates(uvArray, numTriangles*2);
	}

	loadVBO();
}