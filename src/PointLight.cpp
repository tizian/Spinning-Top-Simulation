#include "PointLight.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight() {
    m_position = glm::vec3(0, 0, 0);
    m_color = glm::vec3(1, 1, 1);
}

PointLight::PointLight(const glm::vec3 & position) {
    m_position = glm::vec3(position);
    m_color = glm::vec3(1, 1, 1);
}

PointLight::PointLight(const glm::vec3 & position, const glm::vec3 & color) {
    m_position = glm::vec3(position);
    m_color = glm::vec3(color);
}

void PointLight::setPosition(const glm::vec3 & position) {
	m_position = position;
}

void PointLight::setColor(const glm::vec3 & color) {
	m_color = color;
}

glm::vec3 PointLight::getPosition() const {
	return m_position;
}

glm::vec3 PointLight::getColor() const {
	return m_color;
}

void PointLight::setUniforms(Shader * shader) {
	shader->use();
    shader->setUniform("light.position", m_position);
    shader->setUniform("light.color", m_color);
}