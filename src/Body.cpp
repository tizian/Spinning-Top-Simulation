#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Body::Body(const glm::vec3 & position, const glm::quat & orientation, const glm::vec3 & scale) {
	m_position = position;
	m_orientation = orientation;
	m_scale = scale;
    
    m_boundingBox = AABB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
    
    m_debugMesh = nullptr;
    m_debugMaterial = nullptr;
}

Body::Body(const glm::vec3 & position) {
	m_position = position;
    m_orientation = glm::quat();
	m_scale = glm::vec3(1, 1, 1);
    
    m_boundingBox = AABB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
}

Body::Body() {
	m_position = glm::vec3();
    m_orientation = glm::quat();
	m_scale = glm::vec3(1, 1, 1);
    
    m_boundingBox = AABB();
    
    m_mesh = nullptr;
    m_material = nullptr;
    m_texture = nullptr;
}

void Body::calculateBoundingBox()
{
    vec3 origin = vec3(0,0,0);
    vec3 radii = vec3(0,0,0);
    
    if (m_mesh != nullptr)
    {
        GLfloat * vertices = m_mesh->getVertices();
        
        for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
        {
            vec3 vertex = vec3(vertices[i], vertices[i+1], vertices[i+2]);
            
            origin = vec3(min(origin.x, vertex.x), min(origin.y, vertex.y), min(origin.z, vertex.z));
            radii = vec3(max(radii.x, vertex.x - origin.x), max(radii.y, vertex.y - origin.y), max(radii.z, vertex.z - origin.z));
        }
    }
    
    // add offset to ensure enclosure
    m_boundingBox.origin = origin - vec3(0.1, 0.1, 0.1);
    m_boundingBox.radii = radii + vec3(0.2, 0.2, 0.2);
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
    calculateBoundingBox();
}

void Body::setTexture(Texture * texture) {
    m_texture = texture;
}

void Body::setDebugMaterial(Material * material) {
    m_debugMaterial = material;
}

void Body::setDebugMesh(Mesh * mesh) {
    m_debugMesh = mesh;
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

AABB Body::getBoundingBox() {
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
    
    m_material->setUniforms();
    
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
    mat4 debugModel = translation() * glm::scale(mat4(), vec3(0.05));
    
    Shader::setUniform("modelMatrix", debugModel);
    
    m_debugMaterial->setUniforms();
    
    m_debugMesh->render();
}

void Body::update(float dt) {}