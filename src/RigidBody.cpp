#include "RigidBody.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/ext.hpp>

#include <algorithm>

vec3 maxAngularVelocity = vec3(1,1,1) * 100000000.f; // 100 000 000 is an arbitrary but resonable limit to avoid nan

bool firstTime;

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
    int sampleCount = 1000000; // should be higher! But then we need to avoid calculating everything when 1, 2 or 3 is pressed.
    mat3 bodyInertiaTensor = mat3({0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f});
    
    int realSampleCount = 0;
    
    if (m_mesh != nullptr)
    {
        vec3 origin = getBoundingBox().origin;
        vec3 radii = getBoundingBox().radii;
        
        int numVertices = m_mesh->getNumVertices();
        GLfloat * vertices = m_mesh->getVertices();
        
        // do monte-carlo integration by using sampleCount samples inside bounding box
        for (int i = 0; i < sampleCount; ++i)
        {
            // get sample inside boundingbox
            vec3 sample = origin + vec3((float)rand() / (float)RAND_MAX * radii.x, (float)rand() / (float)RAND_MAX * radii.y, (float)rand() / (float)RAND_MAX * radii.z);
            //printf("sample: %f %f %f\n", sample.x, sample.y, sample.z);
            vec3 rayOrigin = origin;
            vec3 rayDirection = sample - origin;
            float theT = sqrt(rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z); // needed later
            rayDirection /= theT;
            std::vector<float> ts = std::vector<float>();
            
            //printf("theT: %f\n", theT);
            
            for (int j = 0; j < numVertices; j += 9)
            {
                vec3 vertex1 = vec3(vertices[j], vertices[j+1], vertices[j+2]);
                vec3 vertex2 = vec3(vertices[j+3], vertices[j+4], vertices[j+5]);
                vec3 vertex3 = vec3(vertices[j+6], vertices[j+7], vertices[j+8]);
                
                //printf("v1: %f %f %f\nv2: %f %f %f\nv3: %f %f %f\n");
                
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
                //printf("ts[%d]: %f\n", j, ts[j]);
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
        
    }
    
    if (m_mesh == nullptr || realSampleCount == 0) {
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
    firstTime = true;
}

void RigidBody::printState()
{
    printf("m_position: %f %f %f\n", m_position.x, m_position.y, m_position.z);
    printf("m_orientation: %f %f %f %f\n", m_orientation.w, m_orientation.x, m_orientation.y, m_orientation.z);
    printf("m_angularVelocity: %f %f %f\n", m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);
}

void RigidBody::setBodyInertiaTensorInv(const mat3 bodyInertiaTensorInv) {
    m_bodyInertiaTensorInv = bodyInertiaTensorInv;
}

void RigidBody::setMesh(Mesh *mesh) {
    Body::setMesh(mesh);
//    calculateInertiaTensor();
//    printf("body inertia tensor inv:\n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n", m_bodyInertiaTensorInv[0][0], m_bodyInertiaTensorInv[0][1], m_bodyInertiaTensorInv[0][2], m_bodyInertiaTensorInv[1][0], m_bodyInertiaTensorInv[1][1], m_bodyInertiaTensorInv[1][2], m_bodyInertiaTensorInv[2][0], m_bodyInertiaTensorInv[2][1], m_bodyInertiaTensorInv[2][2]);
}

void RigidBody::addForce(const vec3 force) {
    addForce(force, m_position);
}

void RigidBody::addForce(const vec3 force, const vec3 position) {
    // PBS slides
    m_force += force;
    vec3 taui = glm::cross(position - m_position, force);

    m_torque += taui;  // Add torque: but depends on location of force... ?
}

// assume ground at (x, 0, z)
float RigidBody::distanceToGround()
{
    GLfloat dist = MAXFLOAT;
    GLfloat * vertices = m_mesh->getVertices();
    //printf("NumVertices: %d\n", m_mesh->getNumVertices());
    
    for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
    {
        vec4 tmp = model() * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.f);
        //printf("x: %f y: %f z: %f\n", tmp.x, tmp.y, tmp.z);
        if (tmp.y < dist)
        {
            //printf("relevant vertex: %d\n", i/3);
            dist = tmp.y;
        }
    }
    
    return dist;
}

// assume ground at (x, 0, z)
// returns the colliding vertices with their world coordinates
// The first entry is the one for collision response. The others are for the friction
std::vector<vec3> RigidBody::intersectWithGround()
{
//    vec3 normal = vec3(0,1,0);
    std::vector<vec3> points = std::vector<vec3>();
    
    GLfloat * vertices = m_mesh->getVertices();
//    GLfloat * normals = m_mesh->getNormarls();
    
    for (int i = 0; i < m_mesh->getNumVertices(); i += 3)
    {
        vec3 vertex = vec3(vertices[i], vertices[i+1], vertices[i+2]); // body space
        vec4 tmp = model() * vec4(vertex.x, vertex.y, vertex.z, 1.0f); // world space
        vertex = vec3(tmp.x, tmp.y, tmp.z);
        
//        tmp = transpose(inverse(model())) * vec4(normals[i], normals[i+1], normals[i+2], 1.f);
//        vec3 vertexNormal = vec3(tmp.x, tmp.y, tmp.z);
        if (vertex.y < 0)
        {
            if (points.empty()) {
                points.push_back(vertex);
            } else if (points[0].y == vertex.y && std::find(points.begin(), points.end(), vertex) == points.end()) {
                points.push_back(vertex);
            } else if (points[0].y > vertex.y) {
                points = std::vector<vec3>();
                points.push_back(vertex);
            }
        }
    }
    
    vec3 point = vec3(); //final point
    
    // get the point closest to the COM
    if (points.size() > 2) {
        // face - ground collision; May also be a virtual face
        if (m_position.y == points[0].y) {
            point = m_position;
        } else {
            vec3 direction = cross(points[0] - points[2], points[1] - points[2]);
            float distance = (points[0].y - m_position.y) / direction.y; // m_position + distance * direction = point[0].y
            point = m_position + distance * direction;
        }
    } else if (points.size() == 2) {
        // line - ground collision; May also be a virtual line
        // http://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
        vec3 line = points[0] - points[1];
        float distance = dot(m_position, line)/dot(line, line); // distance from points[1] to the projected point.
        point = distance * line + points[1];

        if (distance < 0) {
            point = points[1];
        } else if (distance > 1) {
            point = points[0];
        }
        
//        printf("LINE:\npoints[0]: %f %f %f\npoints[1]: %f %f %f\npoint: %f %f %f\n", points[0].x, points[0].y, points[0].z, points[1].x, points[1].y, points[1].z, point.x, point.y, point.z);
    }
    
    if (points.size() > 1) {
        /*points = std::vector<vec3>();
        points.push_back(point);*/
        points.insert(points.begin(), point);
    }
    
    return points;
}

void RigidBody::update(float dt) {
    
    // Gravity
    addForce(vec3(0, -9.81 * m_mass, 0));  // hardcoded hack
    
    // Update state with euler integration step

    m_position = m_position + dt * m_linearMomentum / m_mass;                               // x(t) = x(t) + dt * M^-1 * P(t)
    
    quat omega = quat(1.0, m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);  // Convert omega(t) to a quaternion to do rotation
    m_orientation = m_orientation + 0.5f * dt * omega * m_orientation;                      // q(t) = q(t) + dt * 1/2 * omega(t) * q(t)
    
    m_linearMomentum = m_linearMomentum + dt * m_force;                                                                     // P(t) = P(t) + dt * F(t)
    m_rotationMatrix = mat3(mat3_cast(m_orientation));                                                                      // convert quaternion q(t) to matrix R(t)
    m_angularMomentum = m_angularMomentum + dt * m_torque;                                                                  // L(t) = L(t) + dt * tau(t)
    m_inertiaTensorInv = m_rotationMatrix * m_bodyInertiaTensorInv * transpose(m_rotationMatrix);                           // I(t)^-1 = R(t) * I_body^-1 * R(t)'
    m_angularVelocity = min(max(m_inertiaTensorInv * m_angularMomentum, -1.f * maxAngularVelocity), maxAngularVelocity);    // omega(t) = I(t)^-1 * L(t)
//    m_linearVelocity = m_linearMomentum / m_mass;
    
    float distanceGround = distanceToGround();
    vec3 normal = vec3(0, 1, 0);
    
    // check for ground collision and do collision response
    if (distanceGround < 0)
    {
        if (firstTime) {
            printf("First contact with ground:\n");
        }
        
        std::vector<vec3> collisionPoints = intersectWithGround();
        //printf("collisionPoints.size: %lu\n", collisionPoints.size());
        
        vec3 org_linearMomentum = m_linearMomentum;
        

            // Impulse-Based Collision Response
            
//            printf("collision point: %f %f %f\n", collisionPoints[i].x, collisionPoints[i].y, collisionPoints[i].z);
            vec3 r = collisionPoints[0] - m_position;   // r_a = p - x(t)
            vec3 v = org_linearMomentum / m_mass + cross(m_angularVelocity, r);
            
            vec3 vrel = v - vec3(0, 0, 0);    // v_r = v_p2 - v_p1
            
            if (firstTime) {
                printf("\tvrel: %f %f %f\n", vrel.x, vrel.y, vrel.z);
            }
            
            // Colliding contact
            
            float e = 0.3;  // Coefficient of restitution
            float j = -(1.f+e)*dot(vrel, normal)/(1.f/m_mass + dot(normal, cross(m_inertiaTensorInv * cross(r, normal), r)));
            j = max(0.0f, j);   // not sure... part of 'Realtime Rigid Body Simulation Using Impulses' paper
//            printf("j: %f\n", j);
            
            vec3 impulse = j * normal;
            impulse = impulse * (1.f/collisionPoints.size());  // In theory: only one collision point...
//            printf("impulse: %f %f %f\n", impulse.x, impulse.y, impulse.z);
            
            vec3 torqueImpulse = cross(r, impulse);
//            printf("torqueImpulse: %f %f %f\n", torqueImpulse.x, torqueImpulse.y, torqueImpulse.z);
            
            m_linearMomentum = m_linearMomentum + impulse;
            m_angularMomentum = m_angularMomentum + torqueImpulse;
            
        for (int i = 0; i < collisionPoints.size(); i++)
        {
            // Impulse-Based Friction Model (Coulomb friction model)
            
            float mu = 0.8; // wild guess
            vec3 tangent = cross(cross(normal, vrel), normal)/length(vrel);
            
            float jt = -(1.f+e)*dot(vrel, tangent)/(1.f/m_mass + dot(tangent, cross(m_inertiaTensorInv * cross(r, tangent), r)));
            
            jt = clamp(jt, -mu*j, mu*j);
            
            vec3 frictionImpulse = jt * tangent;
            frictionImpulse = frictionImpulse * (1.f/collisionPoints.size());  // In theory: only one collision point...
            
            vec3 torqueFrictionImpulse = cross(r, frictionImpulse);
            
            m_linearMomentum = m_linearMomentum + frictionImpulse;
            m_angularMomentum = m_angularMomentum + torqueFrictionImpulse;
        }
        
        
        // avoid overshooting and undershooting
        m_position.y -= distanceGround;
        
        if (firstTime) {
            printf("\tcollision points: %lu\n", collisionPoints.size());
            vec3 linearImpulse = m_linearMomentum - org_linearMomentum;
            printf("\tlinear impulse: %f %f %f\n", linearImpulse.x, linearImpulse.y, linearImpulse.z);
            printf("\tm_linearMomentum: %f %f %f\n", m_linearMomentum.x, m_linearMomentum.y, m_linearMomentum.z);
            firstTime = false;
        }
    }
    
    // Fake slowing down
//    m_angularMomentum *= 0.999f;
//    m_linearMomentum *= 0.999f;
    
    // Reset forces and torque
    m_force = glm::vec3(0, 0, 0);
    m_torque = glm::vec3(0, 0, 0);
    
    // normalize orientation quaternion
    m_orientation = normalize(m_orientation);
    
    //printState();
}
