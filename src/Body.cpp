#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Body::Body(const glm::vec3 & position, const glm::quat & orientation, const glm::vec3 & scale) {
	m_position = position;
	m_orientation = orientation;
	m_scale = scale;
    
    m_boundingBox = OOBB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
    
    m_boundingBoxMesh = nullptr;
    m_centerOfMassMesh = nullptr;
    m_debugMaterial = nullptr;
}

Body::Body(const glm::vec3 & position) {
	m_position = position;
    m_orientation = glm::quat();
	m_scale = glm::vec3(1, 1, 1);
    
    m_boundingBox = OOBB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
    
    m_boundingBoxMesh = nullptr;
    m_centerOfMassMesh = nullptr;
    m_debugMaterial = nullptr;
}

Body::Body() {
	m_position = glm::vec3();
    m_orientation = glm::quat();
	m_scale = glm::vec3(1, 1, 1);
    
    m_boundingBox = OOBB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
    
    m_boundingBoxMesh = nullptr;
    m_centerOfMassMesh = nullptr;
    m_debugMaterial = nullptr;
}

void Body::setPosition(const glm::vec3 & position) {
	m_position = position;
}

void Body::setOrientation(const glm::quat & orientation) {
	m_orientation = normalize(orientation);
}

void Body::setScale(const glm::vec3 & scale) {
	m_scale = scale;
}

void Body::setMaterial(Material * material) {
	m_material = material;
}

void Body::setMesh(Mesh * mesh) {
    m_mesh = mesh;
    m_boundingBox = OOBB(mesh);
}

void Body::setTexture(Texture * texture) {
    m_texture = texture;
}

void Body::setDebugMaterial(Material * material) {
    m_debugMaterial = material;
}

void Body::setCenterOfMassMesh(Mesh * mesh) {
    m_centerOfMassMesh = mesh;
}

glm::vec3 Body::getPosition() const {
	return m_position;
}

glm::quat Body::getOrientation() const {
    return m_orientation;
}

glm::vec3 Body::getScale() const {
	return m_scale;
}

Material * Body::getMaterial() const {
	return m_material;
}

Mesh * Body::getMesh() const {
    return m_mesh;
}

Texture * Body::getTexture() const {
    return m_texture;
}

OOBB Body::getBoundingBox() {
    return m_boundingBox;
}

glm::mat4 Body::translation() const {
	return glm::translate(glm::mat4(), m_position);
}

glm::mat4 Body::scale() const {
	return glm::scale(glm::mat4(), m_scale);
}

glm::mat4 Body::rotation() const {
    return glm::mat4_cast(m_orientation);
}

glm::mat4 Body::model() const {
	return translation() * rotation() * scale();
}

void Body::render() {
    
    Shader::setUniform("modelMatrix", model());
    
    if (m_material != nullptr) {
        m_material->setUniforms();
    }
    
    if (m_texture != nullptr) {
        m_texture->use();
        Shader::setUniform("tex", m_texture->getTextureUnit());
    }
    
    
	if (m_mesh == nullptr) {
		printf("ERROR: Can't render ModelInstance. Mesh not set.\n");
		exit(-1);
	}
	m_mesh->render();
}

void Body::debugRender() {
    mat4 debugModel = translation() * glm::scale(mat4(), vec3(0.1));
    
    Shader::setUniform("modelMatrix", debugModel);
    
    m_debugMaterial->setUniforms();
    
    m_centerOfMassMesh->render();
}

void Body::update(float dt) {}