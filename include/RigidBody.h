#pragma once

#include "Body.h"

class RigidBody : public Body {
public:
    RigidBody(const vec3 & position, const quat & orientation, const vec3 & scale);
    RigidBody(const vec3 & position);
    RigidBody();
    
    virtual void update(float dt);
    
    void addForce(const vec3 force);
    void addForce(const vec3 force, const vec3 position);
    
private:
    void setDefaults();
    
    // constant values
    virtual mat3 getBodyInertiaTensorInv() const;  // Override for all rigid bodies: depends on shape
    virtual vec3 getCenterOfMass() const;          // Override for all rigid bodies: depends on shape
    float m_mass;  // mass
    
    // state
    vec3 m_linearMomentum;      // P(t) = M * v(t)
    vec3 m_angularMomentum;     // L(t) = I(t) * omega(t)
    
    // derived quantities
//    vec3 m_linearVelocity;      // v(t) -> not used
    vec3 m_angularVelocity;     // omega(t)
    mat3 m_inertiaTensorInv;    // I(t)^-1
    mat3 m_rotationMatrix;      // R(t)
    
    // computed quantities
    vec3 m_force;               // F(t)
    vec3 m_torque;              // tau(t)
};