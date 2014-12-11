#pragma once

#include "Body.h"
#include "Contact.h"

#include <vector>

class RigidBody : public Body {
public:
    RigidBody(const glm::vec3 & position, const glm::quat & orientation, const glm::vec3 & scale);
    RigidBody(const glm::vec3 & position);
    RigidBody();
    
    virtual void update(float dt);
    
    virtual void setMesh(Mesh *mesh);
    void setBodyInertiaTensorInv(const glm::mat3 bodyInertiaTensorInv);
    
    OOBB * getBoundingBox();
    
    void addForce(const glm::vec3 force);
    void addForce(const glm::vec3 force, const glm::vec3 position);
    
    void addImpulse(const glm::vec3 impulse);
    void addImpulse(const glm::vec3 impulse, const glm::vec3 position);
    
    int type; // sphere, cube, sp1, etc...
    
    bool isCurrentlyActive;
    
    std::vector<Contact> intersectWith(RigidBody & body);
    
    void renderOctree();
    
private:
    bool m_active;
    
    void setDefaults();
    
    void printState();
    
    float distanceToGround();
    std::vector<glm::vec3> intersectWithGround();
    
    // constant values
    //virtual mat3 getBodyInertiaTensorInv() const;  // Override for all rigid bodies: depends on shape
    float m_mass;  // mass
    glm::mat3 m_bodyInertiaTensorInv;
    
    // state
    glm::vec3 m_linearMomentum;      // P(t) = M * v(t)
    glm::vec3 m_angularMomentum;     // L(t) = I(t) * omega(t)
    
    // derived quantities
//    vec3 m_linearVelocity;      // v(t)
    glm::vec3 m_angularVelocity;     // omega(t)
    glm::mat3 m_inertiaTensorInv;    // I(t)^-1
    glm::mat3 m_rotationMatrix;      // R(t)
    
    // computed quantities
    glm::vec3 m_force;               // F(t)
    glm::vec3 m_torque;              // tau(t)
    
    std::vector<float> m_lastVelocities;
    
    OOBB * m_boundingBox;
    
    std::vector<Body> * octreeMeshes;
};