#include "OOBB.h"

OOBB::OOBB()
{
    setDefaults();
}

OOBB::OOBB(std::vector<glm::vec3> includedTriangles, glm::vec3 origin, glm::vec3 radii)
{
    setDefaults();
    
    m_includedTriangles = includedTriangles;
    
    m_origin = origin;
    m_radii = radii;
}

OOBB::OOBB(Mesh * mesh) {
    setDefaults();
    
    std::vector<glm::vec3> includedTriangles = std::vector<glm::vec3>();
    
    for (int i = 0; i < mesh->getNumVertices(); i += 3)
    {
        glm::vec3 vertex = glm::vec3(mesh->getVertices()[i], mesh->getVertices()[i+1], mesh->getVertices()[i+2]);
        includedTriangles.push_back(vertex);
    }
    
    bool equalVerticesInSameTriangle = false;
    GLfloat * vertices = mesh->getVertices();
    for (int i = 0; i < mesh->getNumVertices(); i += 9) {
        glm::vec3 vertex1 = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        glm::vec3 vertex2 = glm::vec3(vertices[i+3], vertices[i+4], vertices[i+5]);
        glm::vec3 vertex3 = glm::vec3(vertices[i+6], vertices[i+7], vertices[i+8]);
        
        if (vertex1 == vertex2 || vertex1 == vertex3 || vertex2 == vertex3)
        {
            equalVerticesInSameTriangle = true;
        }
    }
    
    m_includedTriangles = includedTriangles;
    calculateBoundingBox();
    
    if (!equalVerticesInSameTriangle)
    {
        
        split();
        printf("OOBB includedTriangles: %lu radii: %f %f %f\n", includedTriangles.size()/3, m_radii.x, m_radii.y, m_radii.z);
        int sum = 0;
        for (int i = 0; i < children.size(); ++i) {
            sum += children[i].getIncludedTriangles().size()/3;
            printf("\tchild: %d includedTriangles: %lu radii %f %f %f\n", i, children[i].getIncludedTriangles().size()/3, children[i].getRadii().x, children[i].getRadii().y, children[i].getRadii().z);
        }
        printf("\tSum of triangles in children: %d\n",sum);
    } else {
        printf("ERROR: Could not create Octree, because there are equal vertices in the same triangles.\n");
    }
}

glm::vec3 OOBB::getOrigin() {
    return m_origin;
}


glm::vec3 OOBB::getRadii() {
    return m_radii;
}

GLuint OOBB::getNumVertices() {
    return 3*8; // To be consistent with Mesh.cpp
}

GLfloat * OOBB::getVertices() {
    return m_vertices;
}

std::vector<glm::vec3> OOBB::getIncludedTriangles() {
    return m_includedTriangles;
}

std::vector<OOBB> OOBB::getChildren()
{
    return children;
}

void OOBB::split() {
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
                if (IntersectionTest::intersectionTriangleBox(point1, point2, point3, origins[j], childRadii)) {
                    triangles[j].push_back(point1);
                    triangles[j].push_back(point2);
                    triangles[j].push_back(point3);
                }
            }
        }
        
        for (int i = 0; i < triangles.size(); ++i) {
            children.push_back(OOBB(triangles[i], origins[i], childRadii));
        }
    }
}

void OOBB::setDefaults() {
    m_origin = glm::vec3(0,0,0);
    m_radii = glm::vec3(0,0,0);
    
    std::vector<GLfloat> m_includedTriangles = std::vector<GLfloat>();
    children = std::vector<OOBB>();
}

void OOBB::calculateBoundingBox() {
    std::vector<glm::vec3> includedTriangles = m_includedTriangles;
    
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






