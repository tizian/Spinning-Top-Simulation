#pragma once

#include <glm/glm.hpp>

struct Triangle {
    glm::vec3 vertex1;
    glm::vec3 vertex2;
    glm::vec3 vertex3;
    
    glm::vec3 normal;
};