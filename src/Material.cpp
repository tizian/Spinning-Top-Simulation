#include "Material.h"

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

Material::Material(const glm::vec3 &specularFactor, const glm::vec3 &diffuseFactor, const glm::vec3 &ambientFactor, float shininess) {
    m_Ks = glm::vec3(specularFactor);
    m_Kd = glm::vec3(diffuseFactor);
    m_Ka = glm::vec3(ambientFactor);
    m_shininess = shininess;
}

Material::Material(const glm::vec3 &color) {
    setColor(color);
}

Material::Material() {
    m_Ks = glm::vec3(1, 1, 1);
    m_Kd = glm::vec3(1, 1, 1);
    m_Ka = glm::vec3(1, 1, 1);
    m_shininess = 100;
}

void Material::setColor(const glm::vec3 &color) {
    m_Ks = glm::vec3(1, 1, 1);
    m_Kd = color;
    m_Ka = glm::vec3(0.6 * color.x, 0.6 * color.y, 0.6 * color.z);
    m_shininess = 100;
}

void Material::setSpecularFactor(const glm::vec3 &specularFactor) {
    m_Ks = specularFactor;
}

void Material::setDiffuseFactor(const glm::vec3 &diffuseFactor) {
    m_Kd = diffuseFactor;
}

void Material::setAmbientFactor(const glm::vec3 &ambientFactor) {
    m_Ka = ambientFactor;
}

void Material::setShininess(float shininess) {
    m_shininess = shininess;
}

glm::vec3 Material::getSpecularFactor() const {
    return m_Ks;
}

glm::vec3 Material::getDiffuseFactor() const {
    return m_Kd;
}

glm::vec3 Material::getAmbientFactor() const {
    return m_Ka;
}

float Material::getShininess() const {
    return m_shininess;
}

void Material::setUniforms() {
    Shader::setUniform("material.Ks", m_Ks);
    Shader::setUniform("material.Kd", m_Kd);
    Shader::setUniform("material.Ka", m_Ka);
    Shader::setUniform("material.shininess", m_shininess);
}