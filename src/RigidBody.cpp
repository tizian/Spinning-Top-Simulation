#include "RigidBody.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/ext.hpp>

#include <algorithm>

mat3 star(const vec3 v) {
    glm::mat3 m = glm::mat3();
    m[0] = glm::vec3(0, v.z, -v.y);
    m[1] = glm::vec3(-v.z, 0, v.x);
    m[2] = glm::vec3(v.y, -v.x, 0);
    return m;
}

// http://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
int triangle_intersection( const vec3   V1,  // Triangle vertices
                          const vec3   V2,
                          const vec3   V3,
                          const vec3    O,  //Ray origin
                          const vec3    D,  //Ray direction
                          float* out )
{
    vec3 e1, e2;  //Edge1, Edge2
    vec3 P, Q, T;
    float det, inv_det, u, v;
    float t;
    float eps = 0.000001;
    
    //Find vectors for two edges sharing V1
    e1 = V2 - V1;
    e2 = V3 - V1;
    //Begin calculating determinant - also used to calculate u parameter
    P = cross(D, e2);
    //if determinant is near zero, ray lies in plane of triangle
    det = dot(e1, P);
    //NOT CULLING
    if(det > -eps && det < eps) return 0;
    inv_det = 1.f / det;
    
    //calculate distance from V1 to ray origin
    T = O - V1;
    
    //Calculate u parameter and test bound
    u = dot(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return 0;
    
    //Prepare to test v parameter
    Q = cross(T, e1);
    
    //Calculate V parameter and test bound
    v = dot(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f) return 0;
    
    t = dot(e2, Q) * inv_det;
    
    if(t > eps) { //ray intersection
        *out = t;
        return 1;
    }
    
    // No hit, no win
    return 0;
}

// using the raytracing approach
void RigidBody::calculateInertiaTensor()
{
    int sampleCount = 1000; // should be higher! But then we need to avoid calculating everything when 1, 2 or 3 is pressed.
    mat3 bodyInertiaTensor = mat3({0,0,0,0,0,0,0,0,0});
    
    if (m_mesh != nullptr)
    {
        vec3 origin = getBoundingBox().origin;
        vec3 radii = getBoundingBox().radii;
        
        int numVertices = m_mesh->getNumVertices();
        GLfloat * vertices = m_mesh->getVertices();
        
        int realSampleCount = 0;
        
        // do monte-carlo integration by using sampleCount samples inside bounding box
        for (int i = 0; i < sampleCount; ++i)
        {
            // get sample inside boundingbox
            vec3 sample = origin + vec3((float)rand() / (float)RAND_MAX * radii.x, (float)rand() / (float)RAND_MAX * radii.x, (float)rand() / (float)RAND_MAX * radii.x);
            //printf("sample: %f %f %f\n", sample.x, sample.y, sample.z);
            vec3 rayOrigin = origin;
            vec3 rayDirection = sample - origin;
            float theT = sqrt(rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z); // needed later
            rayDirection /= theT;
            std::vector<float> ts = std::vector<float>();
            
            for (int j = 0; j < numVertices; j += 9)
            {
                vec3 vertex1 = vec3(vertices[j], vertices[j+1], vertices[j+2]);
                vec3 vertex2 = vec3(vertices[j+3], vertices[j+4], vertices[j+5]);
                vec3 vertex3 = vec3(vertices[j+6], vertices[j+7], vertices[j+8]);
                
                float t = 0;
                
                if (triangle_intersection(vertex1, vertex2, vertex3, rayOrigin, rayDirection, &t) == 1)
                {
                    ts.push_back(t);
                }
            }
            
            sort(ts.begin(), ts.end());
            
            bool outside = true; // where is our sample point
            
            for (int j = 0; j < ts.size(); ++j)
            {
                if (ts[j] < theT)
                {
                    outside = !outside;
                } else {
                    break;
                }
            }
            
            if (!outside)
            {
                // m_i is 1
                // x(t) is (0,0,0) since we operate in objectspace.
                
                bodyInertiaTensor += mat3({std::pow(sample.y, 2.f)+std::pow(sample.z,2.f), -1 * sample.x * sample.y, -1 * sample.x * sample.z,
                    -1 * sample.y * sample.x, std::pow(sample.x, 2.f) + std::pow(sample.z, 2.f), -1 * sample.y * sample.z,
                    -1 * sample.z * sample.x, -1 * sample.z * sample.y, std::pow(sample.x, 2.f) + std::pow(sample.y, 2.f)});
                
                realSampleCount++;
            }
            
        }
        
        if (realSampleCount > 0)
        {
            bodyInertiaTensor /= (float)realSampleCount;
        }
        
        printf("Hits inside object: %d\n", realSampleCount);
        
    } else {
        bodyInertiaTensor = glm::diagonal3x3(glm::vec3(2.0f/5.0f));
    }
    
    m_bodyInertiaTensorInv = glm::inverse(bodyInertiaTensor);
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

void RigidBody::setMesh(Mesh *mesh) {
    Body::setMesh(mesh);
    calculateInertiaTensor();
    printf("body inertia tensor inv:\n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n", m_bodyInertiaTensorInv[0][0], m_bodyInertiaTensorInv[0][1], m_bodyInertiaTensorInv[0][2], m_bodyInertiaTensorInv[1][0], m_bodyInertiaTensorInv[1][1], m_bodyInertiaTensorInv[1][2], m_bodyInertiaTensorInv[2][0], m_bodyInertiaTensorInv[2][1], m_bodyInertiaTensorInv[2][2]);
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
//    mat4 theMat = mat4_cast(m_orientation);
    mat4 theMat = model();
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
    
//    return dist + m_position.y;
    return dist;
}

// assume ground at (x, 0, z)
// returns the colliding vertices with their world coordinates
std::vector<vec3> RigidBody::intersectWithGround()
{
    std::vector<vec3> points = std::vector<vec3>();
    
    GLfloat * vertices = m_mesh->getVertices();
    for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
    {
//        vec4 tmp = mat4_cast(m_orientation) * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
        vec4 tmp = model() * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
//        if (tmp.y + m_position.y <= 0)
        if (tmp.y <= 0)
        {
//            vec3 candidate = vec3(tmp.x, tmp.y, tmp.z) + m_position;
            vec3 candidate = vec3(tmp.x, tmp.y, tmp.z);
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
            float j = -(1+damping)*vrel/(1/m_mass + dot(normal, cross(m_bodyInertiaTensorInv * cross(ra, normal), ra)));
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
    m_inertiaTensorInv = m_rotationMatrix * m_bodyInertiaTensorInv * transpose(m_rotationMatrix);       // I(t)^-1 = R(t) * I_body^-1 * R(t)'
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
