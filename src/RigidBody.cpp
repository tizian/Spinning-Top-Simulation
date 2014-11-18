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
    addForce(force, m_position);
}

void RigidBody::addForce(const vec3 force, const vec3 position) {
    // PBS slides
    m_force += force;
    vec3 taui = glm::cross(position - m_position, force);
    /*if (taui.x != 0.0f) {
        printf("position: %f %f %f m_position: %f %f %f force: %f %f %f taui: %f %f %f\n", position.x, position.y, position.z, m_position.x, m_position.y, m_position.z, force.x, force.y, force.z, taui.x, taui.y, taui.z);
    }*/
    m_torque += taui;  // Add torque: but depends on location of force... ?
}

// assume ground at (x, 0, z)
float RigidBody::distanceToGround()
{
    GLfloat dist = MAXFLOAT - m_position.y - 1;
    GLfloat * vertices = m_mesh->getVertices();
    //printf("NumVertices: %d\n", m_mesh->getNumVertices());
        mat4 theMat = mat4_cast(m_orientation);
        for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
        {
            vec4 tmp = theMat * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.f);
            //printf("x: %f y: %f z: %f\n", tmp.x, tmp.y, tmp.z);
            if (tmp.y < dist)
            {
                //printf("relevant vertex: %d\n", i/3);
                dist = tmp.y;
            }
        }
    
    return dist + m_position.y;
}

// assume ground at (x, 0, z)
// returns the colliding vertices with their world coordinates
std::vector<vec3> RigidBody::intersectWithGround()
{
    std::vector<vec3> points = std::vector<vec3>();
    
    GLfloat * vertices = m_mesh->getVertices();
    for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
    {
        vec4 tmp = mat4_cast(m_orientation) * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
        if (tmp.y + m_position.y <= 0)
        {
            vec3 candidate = vec3(tmp.x, tmp.y, tmp.z) + m_position;
            if (std::find(points.begin(), points.end(), candidate) == points.end())
            {
                points.push_back(candidate);
            }
        }
    }
    
    return points;
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
        //printf("distanceToGround: %f\n", distanceToGround());
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
        
        std::vector<vec3> collisionPoints = intersectWithGround();
        //printf("collisionPoints.size: %lu\n", collisionPoints.size());
        
        // avoid overshooting and undershooting
        m_position.y -= distanceToGround();
        
        for (int i = 0; i < collisionPoints.size(); i++)
        {
            //printf("collision point: %f %f %f\n", collisionPoints[i].x, collisionPoints[i].y, collisionPoints[i].z);
            // collision response
            // add reflecting force by impulse
            
            vec3 normal = vec3(0, 1, 0);
            float vrel = dot(vec3(0, 1, 0), m_linearMomentum / m_mass);
            
            // Colliding contact
            
            vec3 ra = collisionPoints[i] - m_position;//vec3(0, -1, 0);   // for this special case. General: ra = p - x(t)
            
            float damping = 0.3;
            float j = -(1+damping)*vrel/(1/m_mass + dot(normal, cross(getBodyInertiaTensorInv()*cross(ra, normal), ra)));
            //printf("vrel: %f %f %f j: %f\n", vrel.x, vrel.y, vrel.z, j);
            vec3 impulse = j * glm::vec3(0, 1, 0);
            m_linearMomentum = m_linearMomentum + impulse;
            
            vec3 torqueImpulse = cross(ra, impulse);
            m_angularMomentum = m_angularMomentum + torqueImpulse;
            
            // add friction, depend on collisionPoint[i] and on m_angularVelocity
            vec3 particleVelocity = m_linearMomentum + cross(m_angularVelocity, ra); // http://en.wikipedia.org/wiki/Angular_velocity
            addForce(particleVelocity * -1.f, collisionPoints[i]); // don't know why this works.
        }
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
    return glm::diagonal3x3(glm::vec3(5.0f/2.0f));
}
