#pragma once

#include <glm/glm.hpp>

struct Triangle {
    Triangle(){};
    
    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 n) {
        vertex1 = v1;
        vertex2 = v2;
        vertex3 = v3;
        
        normal = n;
    }
    
    Triangle transformWith(glm::mat4 model) {
        Triangle transformedTriangle;
        transformedTriangle.vertex1 = glm::vec3(model * glm::vec4(vertex1.x, vertex1.y, vertex1.z, 1.f));
        transformedTriangle.vertex2 = glm::vec3(model * glm::vec4(vertex2.x, vertex2.y, vertex2.z, 1.f));
        transformedTriangle.vertex3 = glm::vec3(model * glm::vec4(vertex3.x, vertex3.y, vertex3.z, 1.f));
        
        transformedTriangle.normal = glm::vec3(glm::transpose(glm::inverse(model)) * glm::vec4(normal.x, normal.y, normal.z, 1.f));
    
        return transformedTriangle;
    }

    glm::vec3 vertex1;
    glm::vec3 vertex2;
    glm::vec3 vertex3;
    
    glm::vec3 normal;
};