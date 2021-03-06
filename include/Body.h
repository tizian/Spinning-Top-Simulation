#pragma once

#include "Material.h"
#include "Mesh.h"
#include "OOBB.h"
#include "Shader.h"
#include "Texture.h"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// An Object with transformation, material, texture, ...
class Body {
public:
    Body(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale);
    Body(const glm::vec3 &position);
    Body();
    
    void setPosition(const glm::vec3 &position);
    void setOrientation(const glm::quat &orientation);
    void setScale(const glm::vec3 &scale);
    
    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getScale() const;
    
    virtual void setMesh(Mesh *mesh);
    void setMaterial(Material *material);
    void setTexture(Texture *texture);

    Material *getMaterial() const;
    Mesh *getMesh() const;
    Texture *getTexture() const;
    
    // Returns translation matrix
    glm::mat4 translation() const;

    // Returns scale matrix
    glm::mat4 scale() const;

    // Returns rotation matrix
    glm::mat4 rotation() const;

    // Returns model matrix (model = translation * rotation * scale)
    glm::mat4 model() const;

    // Renders the Model
    void render();
    
    // Updates the object
    virtual void update(float dt);
    
protected:
    glm::vec3 m_position;   // Should be the center of mass
    glm::quat m_orientation;
    
    Mesh *m_mesh;
    
private:
    Material *m_material;
    Texture *m_texture;
    
    glm::vec3 m_scale;
    
    glm::vec3 m_debugPoint;
};