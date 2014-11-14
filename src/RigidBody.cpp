#include "RigidBody.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/ext.hpp>

mat3 star(const vec3 v) {
    glm::mat3 m = glm::mat3();
    m[0] = glm::vec3(0, v.z, -v.y);
    m[1] = glm::vec3(-v.z, 0, v.x);
    m[2] = glm::vec3(v.y, -v.x, 0);
    return m;
}

RigidBody::RigidBody(const vec3 & position, const quat & orientation, const vec3 & scale) : Body(position, orientation, scale) {
    setDefaults();
}

RigidBody::RigidBody(const vec3 & position) : Body(position) {
    setDefaults();
}

RigidBody::RigidBody() : Body() {
    setDefaults();
}

void RigidBody::setDefaults() {
    m_mass = 1;
    m_linearMomentum = vec3(0, 0, 0);
    m_angularMomentum = vec3(0, 0, 0);
    m_angularVelocity = vec3(0, 0, 0);
    m_force = vec3(0, 0, 0);
    m_torque = vec3(0, 0, 0);
}

void RigidBody::addForce(const vec3 force) {
    addForce(force, getCenterOfMass());
}

void RigidBody::addForce(const vec3 force, const vec3 position) {
    m_force += force;
    m_torque += glm::cross(position - getCenterOfMass(), force);  // Add torque: but depends on location of force... ?
}

float RigidBody::distanceToGround()
{
    return m_position.y - 1.f;
}

void RigidBody::update(float dt) {
    float realDt = dt;
    float eps = 0.000001f; // epsilon for dt and dtStep
    //printf("realDt: %f\n", realDt);
    
    // Gravity
    addForce(vec3(0, -9.81 * m_mass, 0));  // hardcoded hack
    
    // Update state with euler integration step
    glm::vec3 org_position = m_position;
    m_position = m_position + dt * m_linearMomentum / m_mass;                                           // x(t) = x(t) + dt * M^-1 * P(t)
    
    // check for ground collision and do collision response
    if (distanceToGround() < 0)
    {
        // Do bilinear search for the dt which leads to the collision.
        
        float minDistance = 0.01;
        float dtStep = 0.5f * dt;
        while (abs(distanceToGround()) > minDistance && dtStep > eps) { // assuming the ground is at (x, 0, z)
            //printf("position.y: %f dt: %f dt_step: %f \n", m_position.y, dt, dt_step);
            if (distanceToGround() < 0.f)
            {
                dt -= dtStep;
            } else {
                dt += dtStep;
            }
            
            m_position = org_position + dt * m_linearMomentum / m_mass;
            
            dtStep = dtStep * 0.5f;
        }
        
        // Hardcode avoid overshooting and undershooting
        m_position.y -= distanceToGround();
        
        // collision response
        // add reflecting force by impulse
        // This can't possibly be right...
        
        vec3 normal = vec3(0, 1, 0);
        vec3 ra = vec3(m_position.x, 0, m_position.z) - (m_position + vec3(0, -1, 0));
        vec3 vrel = m_linearMomentum / m_mass;
        
        float dumping = 0.3;
        float j = -(1+dumping)*length(vrel)/(1 + dot(normal, (getBodyInertiaTensorInv()*cross(cross(ra, normal), ra))));
        //printf("vrel: %f %f %f j: %f\n", vrel.x, vrel.y, vrel.z, j);
        vec3 impulse = j * glm::vec3(0, -1, 0);
        m_linearMomentum = m_linearMomentum + impulse;
        
        vec3 torqueImpulse = cross(ra - getCenterOfMass(), impulse);
        m_angularMomentum = m_angularMomentum + torqueImpulse;
    }
    
    m_linearMomentum = m_linearMomentum + dt * m_force;                                                 // P(t) = P(t) + dt * F(t)
    
    quat omega = quat(1.0, m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);              // Convert omega(t) to a quaternion to do rotation
    m_orientation = m_orientation + 0.5f * dt * omega * m_orientation;                                  // q(t) = q(t) + dt * 1/2 * omega(t) * q(t)
    
    m_rotationMatrix = mat3(mat3_cast(m_orientation));                                                  // convert quaternion q(t) to matrix R(t)
    m_angularMomentum = m_angularMomentum + dt * m_torque;                                              // L(t) = L(t) + dt * tau(t)
    m_inertiaTensorInv = m_rotationMatrix * getBodyInertiaTensorInv() * transpose(m_rotationMatrix);    // I(t)^-1 = R(t) * I_body^-1 * R(t)'
    m_angularVelocity = m_inertiaTensorInv * m_angularMomentum;                                         // omega(t) = I(t)^-1 * L(t)
    
    // Reset forces and torque
    m_force = glm::vec3(0, 0, 0);
    m_torque = glm::vec3(0, 0, 0);
    
    // normalize orientation quaternion
    m_orientation = normalize(m_orientation);
    
    //printf("realDt: %f dt: %f\n", realDt, dt);
    if (realDt - dt > eps)
    {
        //printf("realDt - dt: %f\n", realDt - dt);
        // Do the rest of the timestep
        // For now only the time till collision was done.
        update(realDt - dt);
    }
}

glm::mat3 RigidBody::getBodyInertiaTensorInv() const {
    return glm::diagonal3x3(glm::vec3(2.0f/5.0f));
}

glm::vec3 RigidBody::getCenterOfMass() const {
    return m_position;
}