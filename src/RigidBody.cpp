#include "RigidBody.h"

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
    
//    m_bodyInertiaTensorInv = InertiaTensor::calculateInertiaTensor(this);
//    printf("body inertia tensor inv:\n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n", m_bodyInertiaTensorInv[0][0], m_bodyInertiaTensorInv[0][1], m_bodyInertiaTensorInv[0][2], m_bodyInertiaTensorInv[1][0], m_bodyInertiaTensorInv[1][1], m_bodyInertiaTensorInv[1][2], m_bodyInertiaTensorInv[2][0], m_bodyInertiaTensorInv[2][1], m_bodyInertiaTensorInv[2][2]);
}

OOBB RigidBody::getBoundingBox()
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

std::vector<glm::vec3> RigidBody::intersectWith(Body & body)
{
    std::vector<glm::vec3> intersectionPoints = std::vector<glm::vec3>();
    
    mat4 myModel = model();
    mat4 bodyModel = body.model();
    
    if (IntersectionTest::intersectionBoxBox(getBoundingBox().getOrigin(), getBoundingBox().getRadii(), myModel, body.getBoundingBox().getOrigin(), body.getBoundingBox().getRadii(), bodyModel)) {
        
        std::vector<OOBB> myChildren = getBoundingBox().getChildren();
        std::vector<OOBB> myBoundingBoxes = std::vector<OOBB>();
        
        for (int i = 0; i < myChildren.size(); ++i) {
            if (IntersectionTest::intersectionBoxBox(myChildren[i].getOrigin(), myChildren[i].getRadii(), myModel, body.getBoundingBox().getOrigin(), body.getBoundingBox().getRadii(), bodyModel)) {
                myBoundingBoxes.push_back(myChildren[i]);
            }
        }
        
        std::vector<OOBB> bodyChildren = body.getBoundingBox().getChildren();
        std::vector<OOBB> bodyBoundingBoxes = std::vector<OOBB>();
        
        for (int i = 0; i < bodyChildren.size(); ++i) {
            if (IntersectionTest::intersectionBoxBox(bodyChildren[i].getOrigin(), bodyChildren[i].getRadii(), bodyModel, getBoundingBox().getOrigin(), getBoundingBox().getRadii(), myModel)) {
                bodyBoundingBoxes.push_back(bodyChildren[i]);
            }
        }
        
        for (int i = 0; i < myBoundingBoxes.size() && intersectionPoints.size() == 0; ++i) {
            
            for (int j = 0; j < bodyBoundingBoxes.size() && intersectionPoints.size() == 0; ++j) {
                if (IntersectionTest::intersectionBoxBox(myBoundingBoxes[i].getOrigin(), myBoundingBoxes[i].getRadii(), myModel, bodyBoundingBoxes[j].getOrigin(), bodyBoundingBoxes[j].getRadii(), bodyModel))
                {
                    //printf("intersection between:\nbox1\n\torigin: %f %f %f\n\tradii: %f %f %f\n\tm_position: %f %f %f\nbox2\n\torigin: %f %f %f\n\tradii: %f %f %f\n\tm_position: %f %f %f\n", myBoundingBoxes[i].getOrigin().x, myBoundingBoxes[i].getOrigin().y, myBoundingBoxes[i].getOrigin().z, myBoundingBoxes[i].getRadii().x, myBoundingBoxes[i].getRadii().y, myBoundingBoxes[i].getRadii().z, getPosition().x, getPosition().y, getPosition().z, bodyBoundingBoxes[i].getOrigin().x, bodyBoundingBoxes[i].getOrigin().y, bodyBoundingBoxes[i].getOrigin().z, bodyBoundingBoxes[i].getRadii().x, bodyBoundingBoxes[i].getRadii().y, bodyBoundingBoxes[i].getRadii().z, body.getPosition().x, body.getPosition().y, body.getPosition().z);
                    
                    std::vector<glm::vec3> bodyTriangles =  bodyBoundingBoxes[j].getIncludedTriangles();
                    std::vector<glm::vec3> myTriangles =  myBoundingBoxes[i].getIncludedTriangles();
                    
                    glm::vec3 point11;
                    glm::vec3 point12;
                    glm::vec3 point13;
                    
                    glm::vec3 point21;
                    glm::vec3 point22;
                    glm::vec3 point23;
                    
                    for (int p = 0; p < myTriangles.size(); p += 3) {
                        for (int k = 0; k < bodyTriangles.size(); k += 3) {
                            
                            point11 = myTriangles[p];
                            point11 = vec3(myModel * vec4(point11.x, point11.y, point11.z, 1.f));
                            point12 = myTriangles[p+1];
                            point12 = vec3(myModel * vec4(point12.x, point12.y, point12.z, 1.f));
                            point13 = myTriangles[p+2];
                            point13 = vec3(myModel * vec4(point13.x, point13.y, point13.z, 1.f));
                            
                            
                            point21 = bodyTriangles[k];
                            point21 = vec3(bodyModel * vec4(point21.x, point21.y, point21.z, 1.f));
                            point22 = bodyTriangles[k+1];
                            point22 = vec3(bodyModel * vec4(point22.x, point22.y, point22.z, 1.f));
                            point23 = bodyTriangles[k+2];
                            point23 = vec3(bodyModel * vec4(point23.x, point23.y, point23.z, 1.f));
                            
                            glm::vec3 intersectionPoint;
                            
                            if (IntersectionTest::intersectionTriangleTriangle(point11, point12, point13, point21, point22, point23, intersectionPoint))
                            {
                                intersectionPoints.push_back(intersectionPoint);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return intersectionPoints;
}


// assume ground at (x, 0, z)
// only accurate if rigidbody is below the ground, otherwise it returns the distance of the boundingBox to the ground
float RigidBody::distanceToGround()
{
    GLfloat dist = MAXFLOAT;
    GLfloat * vertices = m_boundingBox.getVertices();
    
    mat4 myModel = model();
    
    for (int i = 0; i < m_boundingBox.getNumVertices(); i += 3) {
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
std::vector<vec3> RigidBody::intersectWithGround()
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
    
    return points;
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
//    m_linearVelocity = m_linearMomentum / m_mass;
    
    float distanceGround = distanceToGround();
    vec3 normal = vec3(0, 1, 0);
    
//    printf("m_torque: %f %f %f\n", m_torque.x, m_torque.y, m_torque.z);
//    printf("m_angularVelocity.y: %f\n",m_angularVelocity.y);
    
    // Reset forces and torque
    m_force = glm::vec3(0, 0, 0);
    m_torque = glm::vec3(0, 0, 0);
    
    // check for ground collision and do collision response
    if (distanceGround < 0)
    {
        if (firstTime) {
            printf("First contact with ground:\n\tdistance: %f\n", distanceGround);
        }
        
        double tBeforIntersection = glfwGetTime();
        std::vector<vec3> collisionPoints = intersectWithGround();
        double tAfterIntersection = glfwGetTime();
//        printf("collisionPoints.size: %lu time used: %f\n", collisionPoints.size(), tAfterIntersection - tBeforIntersection);
        
        vec3 org_linearMomentum = m_linearMomentum;
        
        // Impulse-Based Collision Response
        
//        printf("THE collision point: %f %f %f\n", collisionPoints[0].x, collisionPoints[0].y, collisionPoints[0].z);
        vec3 r = collisionPoints[0] - m_position;   // r_a = p - x(t)
        vec3 v = m_linearMomentum / m_mass + cross(m_angularVelocity, r);
        
        vec3 vrel = v - vec3(0, 0, 0);    // v_r = v_p2 - v_p1
        
        if (firstTime) {
            printf("\tvrel: %f %f %f\n", vrel.x, vrel.y, vrel.z);
        }
        
        // Colliding contact
        
        float e = 0.3f;  // Coefficient of restitution
        float j = -(1.f+e)*dot(vrel, normal)/(1.f/m_mass + dot(normal, cross(m_inertiaTensorInv * cross(r, normal), r)));
        j = max(0.0f, j);   // not sure... part of 'Realtime Rigid Body Simulation Using Impulses' paper
//      printf("j: %f\n", j);
        
        vec3 impulse = j * normal;
        
        addImpulse(impulse, collisionPoints[0]);
        
        for (int i = collisionPoints.size() == 1 ? 0 : 1; i < collisionPoints.size(); i++)
        {
            if (frictionMethod == 0) // forced based friction model
            {
                r = collisionPoints[i] - m_position;
                vrel = org_linearMomentum/m_mass + cross(m_angularVelocity, r); // http://en.wikipedia.org/wiki/Angular_velocity

                //printf("bla: %f\n", abs(mat3(model()) * m_angularVelocity).y);
                //if (abs(mat3(model()) * m_angularVelocity).y < 10)
                //{
                    addForce(vrel * 1.f /* (dot(m_angularMomentum, r) / length(part))*/, collisionPoints[i] - vec3(0, distanceGround, 0)); // don't know why this works.
                //m_angularMomentum *= 0.999f;
                //} else {
                //    addForce(vrel * -1.f, collisionPoints[i] - vec3(0, distanceGround, 0)); // don't know why this works.
                //}
            }
            if (frictionMethod == 1)    // Impulse-Based Friction Model (Coulomb friction model)
            {
                r = collisionPoints[i] - m_position;
                vrel = org_linearMomentum/m_mass + cross(m_angularVelocity, r);
                
                float mu = 0.8; // wild guess
                vec3 tangent = cross(cross(normal, vrel), normal)/length(vrel);
                
                float jt = -(1.f+e)*dot(vrel, tangent)/(1.f/m_mass + dot(tangent, cross(m_inertiaTensorInv * cross(r, tangent), r)));
                
                jt = clamp(jt, -mu*j, mu*j);
                
                vec3 frictionImpulse = jt * tangent;
                frictionImpulse = frictionImpulse * (1.f/collisionPoints.size());  // In theory: only one collision point...
                
                addImpulse(frictionImpulse, collisionPoints[i]);
            
            }
            else if (frictionMethod == 3) // MatLab friction
            {
                int collisionPointsSize = collisionPoints.size() == 1 ? 1 : (int)collisionPoints.size() - 1;
                r = collisionPoints[i] - m_position;
                vrel = org_linearMomentum/m_mass + cross(m_angularVelocity, r);
                
                // Torque Friction from http://ch.mathworks.com/help/physmod/simscape/ref/rotationalfriction.html
                
                float frictionTorque; // T
                float coulombFrictionTorque = 20; // T_C; [N*m]
                float breakawayFrictionTorque = 25; // T_brk; [N*m];
                float omega = m_angularVelocity.x; // w
                float velocityThreshold = 0.001; // omega_th; should be between 10^-3 and 10^-5
                float viscousFrictionCoefficient = 0.001; // f; [N*m/(rad/s)];
                float coefficient = 10; // c_v; [rad/s]
                
                if (abs(omega) >= velocityThreshold)
                {
                    frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
                } else {
                    frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                m_torque += 0.1f * vec3(-frictionTorque / collisionPointsSize,0,0);
                
                omega = m_angularVelocity.y;
                
                if (abs(omega) >= velocityThreshold)
                {
                    frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
                } else {
                    frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                m_torque += 0.1f * vec3(0,-frictionTorque / collisionPointsSize,0);
                
                omega = m_angularVelocity.z;
                
                if (abs(omega) >= velocityThreshold)
                {
                    frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
                } else {
                    frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                m_torque += 0.1f * vec3(0,0,-frictionTorque / collisionPointsSize);
                
                // Linear Friction from http://ch.mathworks.com/help/physmod/simscape/ref/translationalfriction.html
                // x component
                float frictionForce; // F
                float coulombFrictionForce = 20; // F_C; [N]
                float breakawayFrictionForce = 25; // F_brk; [N];
                float v = vrel.x;//m_linearMomentum.x / m_mass; // v
                velocityThreshold = 0.0001; // omega_th; should be between 10^-4 and 10^-6
                viscousFrictionCoefficient = 10; // f; [N/(m//s)]; default = 100;
                coefficient = 10; // c_v; [s/m]
                
                if (abs(v) >= velocityThreshold)
                {
                    frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
                } else {
                    frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                
                addForce(0.1f * vec3(-frictionForce / collisionPointsSize,0,0), collisionPoints[i] - vec3(0, distanceGround, 0));
                
                // y component
                v = vrel.y;//m_linearMomentum.z / m_mass; // v
                if (abs(v) >= velocityThreshold)
                {
                    frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
                } else {
                    frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                
                addForce(0.1f * vec3(0,-frictionForce / collisionPointsSize, 0), collisionPoints[i] - vec3(0, distanceGround, 0));
                
                // z component
                v = vrel.z;//m_linearMomentum.z / m_mass; // v
                if (abs(v) >= velocityThreshold)
                {
                    frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
                } else {
                    frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
                }
                
                addForce(0.1f * vec3(0,0,-frictionForce / collisionPointsSize), collisionPoints[i] - vec3(0, distanceGround, 0));
                
            }
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
        
        float vel = length(m_linearMomentum/m_mass + m_angularVelocity);
        m_lastVelocities.push_back(vel);
        if (m_lastVelocities.size() > 10) {
            auto start = m_lastVelocities.end() - 10;
            auto end = m_lastVelocities.end();
            
            m_lastVelocities = std::vector<float>(start, end);
        }
        float average = std::accumulate(m_lastVelocities.begin(), m_lastVelocities.end(), 0.0);
        average /= m_lastVelocities.size();
//        printf("vel: %f\n", average);
        
        if (average < 0.6) m_active = false;
        
//        if (length(m_linearMomentum) < 0.15 && length(m_angularMomentum) < 0.15) m_active = false;
    }
    
    // Fake slowing down
//    m_angularMomentum *= 0.999f;
//    m_linearMomentum *= 0.999f;
    
//    printState();
    
    
}
