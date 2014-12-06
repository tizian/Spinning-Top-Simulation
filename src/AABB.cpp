#include "AABB.h"

bool intersetcion(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 origin, glm::vec3 radii)
{
    return true;
}

AABB::AABB()
{
    setDefaults();
}

AABB::AABB(std::vector<glm::vec3> includedTriangles, glm::vec3 origin, glm::vec3 radii)
{
    setDefaults();
    
    m_origin = origin;
    m_radii = radii;
}

AABB::AABB(Mesh * mesh) {
    setDefaults();
    
    std::vector<glm::vec3> includedTriangles = std::vector<glm::vec3>();
    
    for (int i = 0; i < mesh->getNumVertices(); i += 3)
    {
        glm::vec3 vertex = glm::vec3(mesh->getVertices()[i], mesh->getVertices()[i+1], mesh->getVertices()[i+2]);
        includedTriangles.push_back(vertex);
    }
    
    calculateBoundingBox(includedTriangles);
}

glm::vec3 AABB::getOrigin() {
    return m_origin;
}


glm::vec3 AABB::getRadii() {
    return m_radii;
}

GLuint AABB::getNumVertices() {
    return 3*8; // To be consistent with Mesh.cpp
}

GLfloat * AABB::getVertices() {
    return m_vertices;
}

void AABB::split() {
    if (children.size() == 0) {
        glm::vec3 childRadii = m_radii * 0.5f;
        
        glm::vec3 child1Origin = m_origin;
        glm::vec3 child2Origin = m_origin + glm::vec3(childRadii.x, 0, 0);
        glm::vec3 child3Origin = m_origin + glm::vec3(0, childRadii.y, 0);
        glm::vec3 child4Origin = m_origin + glm::vec3(0, 0, childRadii.z);
        glm::vec3 child5Origin = m_origin + glm::vec3(childRadii.x, childRadii.y, 0);
        glm::vec3 child6Origin = m_origin + glm::vec3(childRadii.x, 0, childRadii.z);
        glm::vec3 child7Origin = m_origin + glm::vec3(0, childRadii.y, childRadii.z);
        glm::vec3 child8Origin = m_origin + glm::vec3(childRadii.x, childRadii.y, childRadii.z);
        
        std::vector<glm::vec3> origins =std::vector<glm::vec3>();
        origins.push_back(child1Origin);
        origins.push_back(child2Origin);
        origins.push_back(child3Origin);
        origins.push_back(child4Origin);
        origins.push_back(child5Origin);
        origins.push_back(child6Origin);
        origins.push_back(child7Origin);
        origins.push_back(child8Origin);
        
        std::vector<glm::vec3> child1Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child2Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child3Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child4Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child5Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child6Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child7Triangles =std::vector<glm::vec3>();
        std::vector<glm::vec3> child8Triangles =std::vector<glm::vec3>();
        
        std::vector<std::vector<glm::vec3> > triangles =std::vector<std::vector<glm::vec3> >();
        triangles.push_back(child1Triangles);
        triangles.push_back(child2Triangles);
        triangles.push_back(child3Triangles);
        triangles.push_back(child4Triangles);
        triangles.push_back(child5Triangles);
        triangles.push_back(child6Triangles);
        triangles.push_back(child7Triangles);
        triangles.push_back(child8Triangles);
        
        for (int i = 0; i < m_includedTriangles.size(); i += 3) {
            glm::vec3 point1 = m_includedTriangles[i];
            glm::vec3 point2 = m_includedTriangles[i+1];
            glm::vec3 point3 = m_includedTriangles[i+2];
            
            for (int j = 0; j < origins.size(); ++j) {
                if (intersetcion(point1, point2, point3, origins[i], childRadii)) {
                    triangles[i].push_back(point1);
                    triangles[i].push_back(point2);
                    triangles[i].push_back(point3);
                }
            }
        }
        
        for (int i = 0; i < triangles.size(); ++i) {
            children.push_back(AABB(triangles[i], origins[i], childRadii));
        }
    }
}

void AABB::setDefaults() {
    m_origin = glm::vec3(0,0,0);
    m_radii = glm::vec3(0,0,0);
    
    std::vector<GLfloat> m_includedTriangles = std::vector<GLfloat>();
    children = std::vector<AABB>();
}

void AABB::calculateBoundingBox(std::vector<glm::vec3> includedTriangles) {
    
    // calculate origin and radii
    m_origin = glm::vec3(0,0,0);
    glm::vec3 m_max = glm::vec3(0,0,0);
    
    for (int i = 0; i < includedTriangles.size(); i++)
    {
        glm::vec3 vertex = includedTriangles[i];
        
        m_origin = glm::vec3(std::min(m_origin.x, vertex.x),std::min(m_origin.y, vertex.y),std::min(m_origin.z, vertex.z));
        m_max = glm::vec3(std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y), std::max(m_max.z, vertex.z));
    }
    
    m_radii = m_max - m_origin;
    
    // add offset to ensure enclosure
    m_origin -= glm::vec3(0.1, 0.1, 0.1);
    m_radii += glm::vec3(0.2, 0.2, 0.2);
    
    // calculate my own vertices
    m_vertices = new GLfloat[3*8];
    
    m_vertices[0] = m_origin.x;
    m_vertices[1] = m_origin.y;
    m_vertices[2] = m_origin.z;
    
    m_vertices[3] = m_origin.x + m_radii.x;
    m_vertices[4] = m_origin.y;
    m_vertices[5] = m_origin.z;
    
    m_vertices[6] = m_origin.x;
    m_vertices[7] = m_origin.y + m_radii.y;
    m_vertices[8] = m_origin.z;
    
    m_vertices[9] = m_origin.x;
    m_vertices[10] = m_origin.y;
    m_vertices[11] = m_origin.z + m_radii.z;
    
    m_vertices[12] = m_origin.x + m_radii.x;
    m_vertices[13] = m_origin.y + m_radii.y;
    m_vertices[14] = m_origin.z;
    
    m_vertices[15] = m_origin.x + m_radii.x;
    m_vertices[16] = m_origin.y;
    m_vertices[17] = m_origin.z + m_radii.z;
    
    m_vertices[18] = m_origin.x;
    m_vertices[19] = m_origin.y + m_radii.y;
    m_vertices[20] = m_origin.z + m_radii.z;
    
    m_vertices[21] = m_origin.x + m_radii.x;
    m_vertices[22] = m_origin.y + m_radii.y;
    m_vertices[23] = m_origin.z + m_radii.z;
}






