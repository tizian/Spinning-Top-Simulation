#pragma once

#include "Mesh.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "IntersectionTest.h"

#include <vector>

class OOBB {
public:
    OOBB();
    OOBB(std::vector<glm::vec3> includedTriangles, glm::vec3 origin, glm::vec3 radii);
    OOBB(Mesh * mesh);
    
    glm::vec3 getOrigin();
    glm::vec3 getRadii();
    
    GLuint getNumVertices();
    GLfloat * getVertices();
    
    std::vector<glm::vec3> getIncludedTriangles();
    std::vector<OOBB> getChildren();
    
    void split();
    
private:
    
    void setDefaults();
    
    void calculateBoundingBox();
    
    glm::vec3 m_origin; // lower left corner
    glm::vec3 m_radii; // width, height, depth
    
    GLfloat * m_vertices = nullptr;
    
    std::vector<glm::vec3> m_includedTriangles; //Triangle: 1,2,3 and 4,5,6 and...
    
    std::vector<OOBB> children;
};