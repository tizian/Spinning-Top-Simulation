#pragma once

#include "IntersectionTest.h"
#include "Mesh.h"
#include "Triangle.h"

#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class OOBB {
public:
    OOBB();
    OOBB(std::vector<Triangle> includedTriangles, glm::vec3 origin, glm::vec3 radii);
    OOBB(Mesh *mesh);
    
    glm::vec3 getOrigin();
    glm::vec3 getRadii();
    
    GLuint getNumVertices();
    GLfloat *getVertices();
    
    int getDepth();
    
    std::vector<Triangle> getIncludedTriangles();
    std::vector<OOBB> *getChildren();
    
    void split(int depth);
    
    void print(bool recursive);
    
private:
    void setDefaults();
    
    void calculateBoundingBox();
    
    void realPrint(int depth);
    
    void setDepths(int depth);
    
    void setBoundingVertices();
    
    glm::vec3 m_origin; // lower left corner
    glm::vec3 m_radii; // width, height, depth
    
    GLfloat *m_vertices = nullptr;
    
    std::vector<Triangle> m_includedTriangles;
    
    std::vector<OOBB> m_children;
    
    int m_depth;
};