#pragma once

#include "Shader.h"

#include <glm/glm.hpp>

// A simple point light.

class PointLight {
public:
    PointLight();
    PointLight(const glm::vec3 &position);
    PointLight(const glm::vec3 &position, const glm::vec3 &color);
    
    void setPosition(const glm::vec3 &position);
    void setColor(const glm::vec3 &color);
    
    glm::vec3 getPosition() const;
    glm::vec3 getColor() const;
    
    // Sets appropriate uniform values for the given shader program
    void setUniforms();
    
private:
    glm::vec3 m_position;
    
    glm::vec3 m_color;
};