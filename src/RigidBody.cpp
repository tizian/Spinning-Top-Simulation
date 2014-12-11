#include "Collision.h"
#include "RigidBody.h"

#ifndef MAXFLOAT
    #define MAXFLOAT      3.40282347e+37F
#endif

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_access.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <numeric>
#include <queue>

using namespace glm;

vec3 maxAngularVelocity = vec3(1,1,1) * 100000000.f; // 100 000 000 is an arbitrary but resonable limit to avoid nan

int frictionMethod = 3; // default = forced based friction; 1 = Impulse-Based Friction Model (Coulomb friction model); NYI 2 = Wikipedia; 3 = MatLab

bool firstTime;

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
    m_active = true;
    m_mass = 1;
    m_linearMomentum = vec3(0, 0, 0);
    m_angularMomentum = vec3(0, 0, 0);
    m_angularVelocity = vec3(0, 0, 0);
    m_force = vec3(0, 0, 0);
    m_torque = vec3(0, 0, 0);
    firstTime = true;
    m_lastVelocities = std::vector<float>();
    isCurrentlyActive = false;
}

void RigidBody::printState()
{
    printf("\n");
    printf("m_position: %f %f %f\n", m_position.x, m_position.y, m_position.z);
    printf("m_orientation: %f %f %f %f\n", m_orientation.w, m_orientation.x, m_orientation.y, m_orientation.z);
    printf("m_linearMomentum: %f %f %f\n", m_linearMomentum.x, m_linearMomentum.y, m_linearMomentum.z);
    printf("m_angularMomentum: %f %f %f\n", m_angularMomentum.x, m_angularMomentum.y, m_angularMomentum.z);
    printf("m_angularVelocity: %f %f %f\n", m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);
    printf("\n");
}

void RigidBody::setBodyInertiaTensorInv(const mat3 bodyInertiaTensorInv) {
    m_bodyInertiaTensorInv = bodyInertiaTensorInv;
}

void RigidBody::setMesh(Mesh *mesh) {
    Body::setMesh(mesh);

    m_boundingBox = new OOBB(mesh);
    
//    m_bodyInertiaTensorInv = InertiaTensor::calculateInertiaTensor(this);
//    printf("body inertia tensor inv:\n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n", m_bodyInertiaTensorInv[0][0], m_bodyInertiaTensorInv[0][1], m_bodyInertiaTensorInv[0][2], m_bodyInertiaTensorInv[1][0], m_bodyInertiaTensorInv[1][1], m_bodyInertiaTensorInv[1][2], m_bodyInertiaTensorInv[2][0], m_bodyInertiaTensorInv[2][1], m_bodyInertiaTensorInv[2][2]);
}

OOBB * RigidBody::getBoundingBox()
{
    return m_boundingBox;
}

void RigidBody::addForce(const vec3 force) {
    addForce(force, m_position);
}

void RigidBody::addForce(const vec3 force, const vec3 position) {
    // PBS slides
    m_force += force;
    
    vec3 taui = glm::cross(position - m_position, force);
    m_torque += taui;
    
    m_active = true;
}

void RigidBody::addImpulse(const vec3 impulse) {
    addImpulse(impulse, m_position);
}

void RigidBody::addImpulse(const vec3 impulse, const vec3 position) {
    m_linearMomentum += impulse;
    
    vec3 torqueImpulse = cross(position - m_position, impulse);
    m_angularMomentum += torqueImpulse;
    
    m_active = true;
    
//    printf("impulse: %f %f %f\n", impulse.x, impulse.y, impulse.z);
//    printf("torqueImpulse: %f %f %f\n", torqueImpulse.x, torqueImpulse.y, torqueImpulse.z);
}

void RigidBody::addTorque(const glm::vec3 torque) {
    m_torque += torque;
}

std::vector<Contact> intersectOctrees(OOBB * one, mat4 & modelOne, OOBB * two, mat4 & modelTwo)
{
    std::vector<Contact> intersectionPoints = std::vector<Contact>();
    
    if (IntersectionTest::intersectionBoxBox(one->getOrigin(), one->getRadii(), modelOne, two->getOrigin(), two->getRadii(), modelTwo)) {
        
        std::vector<OOBB> * childrenOne = one->getChildren();
        std::vector<OOBB> * childrenTwo = two->getChildren();
        
        if (childrenOne->size() > 0 && childrenTwo->size() > 0)
        {
            for (int i = 0; i < childrenOne->size() && intersectionPoints.size() == 0; ++i) {
                for (int j = 0; j < childrenTwo->size() && intersectionPoints.size() == 0; ++j) {
                    intersectionPoints = intersectOctrees(&childrenOne->at(i), modelOne, &childrenTwo->at(j), modelTwo);
                }
            }
        } else if (childrenOne->size() > 0) {
            
            for (int i = 0; i < childrenOne->size() && intersectionPoints.size() == 0; ++i) {
                intersectionPoints = intersectOctrees(&childrenOne->at(i), modelOne, two, modelTwo);
            }
            
        } else if (childrenTwo->size() > 0) {
        
            for (int i = 0; i < childrenTwo->size() && intersectionPoints.size() == 0; ++i) {
                intersectionPoints = intersectOctrees(one, modelOne, &childrenTwo->at(i), modelTwo);
            }
            
        } else {
            std::vector<glm::vec3> trianglesOne =  one->getIncludedTriangles();
            std::vector<glm::vec3> trianglesTwo =  two->getIncludedTriangles();
            
            glm::vec3 point11;
            glm::vec3 point12;
            glm::vec3 point13;
            
            glm::vec3 point21;
            glm::vec3 point22;
            glm::vec3 point23;
            
            for (int i = 0; i < trianglesOne.size(); i += 3) {
                for (int j = 0; j < trianglesTwo.size(); j += 3) {
                    
                    point11 = trianglesOne[i];
                    point11 = vec3(modelOne * vec4(point11.x, point11.y, point11.z, 1.f));
                    point12 = trianglesOne[i+1];
                    point12 = vec3(modelOne * vec4(point12.x, point12.y, point12.z, 1.f));
                    point13 = trianglesOne[i+2];
                    point13 = vec3(modelOne * vec4(point13.x, point13.y, point13.z, 1.f));
                    
                    
                    point21 = trianglesTwo[j];
                    point21 = vec3(modelTwo * vec4(point21.x, point21.y, point21.z, 1.f));
                    point22 = trianglesTwo[j+1];
                    point22 = vec3(modelTwo * vec4(point22.x, point22.y, point22.z, 1.f));
                    point23 = trianglesTwo[j+2];
                    point23 = vec3(modelTwo * vec4(point23.x, point23.y, point23.z, 1.f));
                    
                    glm::vec3 intersectionPoint;
                    glm::vec3 intersectionNormal;
                    
                    if (IntersectionTest::intersectionTriangleTriangle(point11, point12, point13, point21, point22, point23, intersectionPoint, intersectionNormal))
                    {
                        Contact contact;
                        contact.p = intersectionPoint;
                        contact.n = -1.f * intersectionNormal;
                        
                        /*glm::vec3 tmp1 = intersectionNormal * (1.f/length(intersectionNormal));
                        glm::vec3 tmp2 = vec3(modelTwo * vec4(0,0,0,1)) - vec3(modelOne * vec4(0,0,0,1));
                        tmp2 *= -1.f/length(tmp2);
                        
                        printf("cross length: %f\n", length(cross(tmp1, tmp2)));
                        printf("dot: %f\n", dot(tmp1, tmp2));*/
                        
                        intersectionPoints.push_back(contact);
                    }
                }
            }
        }
    }
    
    return intersectionPoints;
}

std::vector<Contact> RigidBody::intersectWith(RigidBody & body)
{
    std::vector<Contact> intersectionPoints = std::vector<Contact>();
    
    mat4 myModel = model();
    mat4 bodyModel = body.model();
    OOBB * myBoundingBox = getBoundingBox();
    OOBB * bodyBoundingBox = body.getBoundingBox();
    intersectionPoints = intersectOctrees(myBoundingBox, myModel, bodyBoundingBox, bodyModel);
    
    return intersectionPoints;
}

// assume ground at (x, 0, z)
// only accurate if rigidbody is below the ground, otherwise it returns the distance of the boundingBox to the ground
float RigidBody::distanceToGround()
{
    GLfloat dist = MAXFLOAT;
    GLfloat * vertices = m_boundingBox->getVertices();
    
    mat4 myModel = model();
    
    for (int i = 0; i < m_boundingBox->getNumVertices(); i += 3) {
        vec4 tmp = myModel * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.f);
//        printf("boundingBox: %f %f %f\n", vertices[i], vertices[i+1], vertices[i+2]);
        if (tmp.y < dist)
        {
            dist = tmp.y;
        }
    }
    
    if (dist < 0)
    {
        dist = MAXFLOAT;
        vertices = m_mesh->getDistinctVertices();
        
        for (int i = 0; i < m_mesh->getNumDistinctVertices(); i += 3)
        {
            vec4 tmp = myModel * vec4(vertices[i], vertices[i+1], vertices[i+2], 1.f);
//            printf("x: %f y: %f z: %f\n", tmp.x, tmp.y, tmp.z);
            if (tmp.y < dist)
            {
//                printf("relevant vertex: %d\n", i/3);
                dist = tmp.y;
            }
        }
    }
    
    return dist;
}

// assume ground at (x, 0, z)
// returns the colliding vertices with their world coordinates
// The first entry is the one for collision response. The others are for the friction
std::vector<Contact> RigidBody::intersectWithGround()
{
//    vec3 normal = vec3(0,1,0);
    std::vector<vec3> points = std::vector<vec3>();
    
    GLfloat * vertices = m_mesh->getDistinctVertices();
    GLuint numVertices = m_mesh->getNumDistinctVertices();
//    GLfloat * normals = m_mesh->getNormarls();
    
    mat4 myModel = model();
    
    for (int i = 0; i < numVertices; i += 3)
    {
        vec3 vertex = vec3(vertices[i], vertices[i+1], vertices[i+2]); // body space
        vec4 tmp = myModel * vec4(vertex.x, vertex.y, vertex.z, 1.0f); // world space
        vertex = vec3(tmp.x, tmp.y, tmp.z);
        
//        tmp = transpose(inverse(model())) * vec4(normals[i], normals[i+1], normals[i+2], 1.f);
//        vec3 vertexNormal = vec3(tmp.x, tmp.y, tmp.z);
        if (vertex.y < 0)
        {
            if (points.empty()) {
                points.push_back(vertex);
            } else if (points[0].y == vertex.y) {
                if (std::find(points.begin(), points.end(), vertex) == points.end()) {
                    points.push_back(vertex);
                }
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
        points.insert(points.begin(), point);
    }
    
    std::vector<Contact> contacts = std::vector<Contact>();
    for (int i = 0; i < points.size(); i++) {
        Contact c;
        c.p = points[i];
        c.n = vec3(0, 1, 0);
        
        contacts.push_back(c);
    }
    
    return contacts;
}

void RigidBody::update(float dt) {
    
    if (!m_active) return;      // Hacked "resting contacts"
    
    // Gravity
    addForce(vec3(0, -9.81 * m_mass, 0));  // hardcoded hack
    
    // Update state with euler integration step

    m_position = m_position + dt * m_linearMomentum / m_mass;                               // x(t) = x(t) + dt * M^-1 * P(t)
    
    quat omega = quat(1.f, m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);  // Convert omega(t) to a quaternion to do rotation
    m_orientation = m_orientation + 0.5f * dt * omega * m_orientation;                      // q(t) = q(t) + dt * 1/2 * omega(t) * q(t)
    
    // normalize orientation quaternion
    m_orientation = normalize(m_orientation);
    
    m_linearMomentum = m_linearMomentum + dt * m_force;                                                                     // P(t) = P(t) + dt * F(t)
    m_rotationMatrix = mat3(mat3_cast(m_orientation));                                                                      // convert quaternion q(t) to matrix R(t)
    m_angularMomentum = m_angularMomentum + dt * m_torque;                                                                  // L(t) = L(t) + dt * tau(t)
    m_inertiaTensorInv = m_rotationMatrix * m_bodyInertiaTensorInv * transpose(m_rotationMatrix);                           // I(t)^-1 = R(t) * I_body^-1 * R(t)'
    m_angularVelocity = min(max(m_inertiaTensorInv * m_angularMomentum, -1.f * maxAngularVelocity), maxAngularVelocity);    // omega(t) = I(t)^-1 * L(t)
    
    float distanceGround = distanceToGround();
    
//    printf("m_torque: %f %f %f\n", m_torque.x, m_torque.y, m_torque.z);
//    printf("m_angularVelocity.y: %f\n",m_angularVelocity.y);
    
    // Reset forces and torque
    m_force = glm::vec3(0, 0, 0);
    m_torque = glm::vec3(0, 0, 0);
    
    // check for ground collision and do collision response
    if (distanceGround < 0)
    {
//        double tBeforIntersection = glfwGetTime();
        
        std::vector<Contact> contacts = intersectWithGround();
        
//        double tAfterIntersection = glfwGetTime();
        
//        printf("collisionPoints.size: %lu time used: %f\n", collisionPoints.size(), tAfterIntersection - tBeforIntersection);
        
        Collision::collisionResponseWithGround(*this, contacts);
        
        m_position.y -= distanceGround;
        
        float vel = length(m_linearMomentum/m_mass + m_angularVelocity);
        m_lastVelocities.push_back(vel);
        if (m_lastVelocities.size() > 10) {
            auto start = m_lastVelocities.end() - 10;
            auto end = m_lastVelocities.end();
            
            m_lastVelocities = std::vector<float>(start, end);
        }
        float average = std::accumulate(m_lastVelocities.begin(), m_lastVelocities.end(), 0.0);
        average /= m_lastVelocities.size();
        
        if (average < 0.8) m_active = false;
     }
    
//    printState();
   
}
