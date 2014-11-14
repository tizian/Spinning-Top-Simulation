#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

// Material properties to be used in a shader

class Material
{
public:
	Material(const glm::vec3 & specularFactor, const glm::vec3 & diffuseFactor, const glm::vec3 & ambientFactor, float specularExp);
    Material(const glm::vec3 & color);
	Material();

	void setSpecularFactor(const glm::vec3 & specularFactor);
	void setDiffuseFactor(const glm::vec3 & diffuseFactor);
	void setAmbientFactor(const glm::vec3 & ambientFactor);
	void setShininess(float shininess);

	glm::vec3 getSpecularFactor() const;
	glm::vec3 getDiffuseFactor() const;
	glm::vec3 getAmbientFactor() const;
	float getShininess() const;

	// Sets appropriate uniform values for the given shader program
	void setUniforms();

private:
	glm::vec3 m_Ks;	// Specular reflectant factor
	glm::vec3 m_Kd;	// Diffuse reflectant factor
	glm::vec3 m_Ka; // Ambient reflectant factor
	float m_shininess;	// Specular exponent
};