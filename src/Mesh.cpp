#include "Mesh.h"

#include <vector>

#include <tiny_obj_loader.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Mesh::Mesh(const std::string &filename) {
    glGenVertexArrays(1, &m_vao);       // Generate a VAO
    glGenBuffers(1, &m_vbo);            // Generate a VBO
    m_vertices = m_distinctVertices = m_normals = m_uvs = m_colors = NULL;
    m_numVertices = m_numDistinctVertices = m_numNormals = m_numUVs = m_numColors = 0;

    loadFromFile(filename);
}

Mesh::Mesh(GLfloat *vertices, int numVertices) {
    if (vertices != nullptr) {
        glGenVertexArrays(1, &m_vao);       // Generate a VAO
        glGenBuffers(1, &m_vbo);            // Generate a VBO
        m_vertices = m_distinctVertices = m_normals = m_uvs = m_colors = NULL;
        m_numVertices = m_numDistinctVertices = m_numNormals = m_numUVs = m_numColors = 0;
        
        setGeometry(vertices, numVertices);
    }
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
    
    std::vector<glm::vec3> distinctVertices = std::vector<glm::vec3>();
    for (int i = 0; i < numVertices; i += 3) {
        glm::vec3 vertex = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        if (std::find(distinctVertices.begin(), distinctVertices.end(), vertex) == distinctVertices.end()) {
            distinctVertices.push_back(vertex);
        }
    }
    
    m_numDistinctVertices = (GLuint)distinctVertices.size() * 3;
    m_distinctVertices = new GLfloat[m_numDistinctVertices];
    
    for (size_t i = 0; i < m_numDistinctVertices; i += 3) {
        m_distinctVertices[i] = distinctVertices[i/3].x;
        m_distinctVertices[i+1] = distinctVertices[i/3].y;
        m_distinctVertices[i+2] = distinctVertices[i/3].z;
    }
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

GLfloat *Mesh::getVertices() {
    return m_vertices;
}

GLuint Mesh::getNumVertices() {
    return m_numVertices;
}

GLfloat *Mesh::getDistinctVertices() {
    return m_distinctVertices;
}

GLuint Mesh::getNumDistinctVertices() {
    return m_numDistinctVertices;
}

GLfloat *Mesh::getNormals(){
    return m_normals;
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

void Mesh::loadFromFile(const std::string &filename) {
    using namespace tinyobj;

    std::vector<shape_t>    shapes;
    std::vector<material_t> materials;

    std::string err = LoadObj(shapes, materials, filename.c_str());
    if (shapes.size() == 0) {
        printf("ERROR: Reading mesh %s. No shape found.\n", filename.c_str());
    } else if (shapes.size() > 1) {
        printf("Warning: Reading mesh %s. Multiple shapes found. Only using the first one.\n", filename.c_str());
    }

    mesh_t &mesh = shapes[0].mesh;

    assert(mesh.normals.size() % 3 == 0);
    size_t normalCount = mesh.normals.size() / 3;
    assert(mesh.texcoords.size() % 2 == 0);
    size_t uvCount = mesh.texcoords.size() / 2;
    assert(mesh.indices.size() % 3 == 0);
    size_t faceCount = mesh.indices.size() / 3;

    float *vertexArray = new float[3*3*faceCount];
    float *normalArray = new float[3*3*faceCount];
    float *uvArray     = new float[2*3*faceCount];

    float *vertexArrayPtr = vertexArray;
    float *normalArrayPtr = normalArray;
    float *uvArrayPtr     = uvArray;

    for (size_t i = 0; i < faceCount; ++i) {
        for (int j = 0; j < 3; ++j) {
            int vertexIdx = mesh.indices[3*i+j];

            if (uvCount > 0) {
                memcpy(uvArrayPtr, &mesh.texcoords[2*vertexIdx], sizeof(float)*2);
                uvArrayPtr += 2;
            }

            if (normalCount > 0) {
                memcpy(normalArrayPtr, &mesh.normals[3*vertexIdx], sizeof(float)*3);
                normalArrayPtr += 3;
            }

            memcpy(vertexArrayPtr, &mesh.positions[3*vertexIdx], sizeof(float)*3);
            vertexArrayPtr += 3;
        }
    }
    
    setGeometry(vertexArray, 3*3*faceCount);
    if (normalCount > 0) {
        setNormals(normalArray, 3*3*faceCount);
    }
    if (uvCount > 0) {
        setTextureCoordinates(uvArray, 2*3*faceCount);
    }

    loadVBO();
}
