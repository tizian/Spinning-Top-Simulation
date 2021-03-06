#pragma once

#include "DebugPoint.h"
#include "RigidBody.h"
#include "RigidBodyFactory.h"

class Simulation {
public:
    Simulation();
    
    void reset();
    
    std::vector<RigidBody> *getCurrentState();
    std::vector<RigidBody> *getLastState();
    
    size_t getNumberOfStates();

    void forwardStep(float dt);
    void backwardStep();
    
    RigidBody *getActiveRigidBody();
    void removeActiveRigidBody();
    void toggleActiveRigidBody();
    void addRigidBody(int type, bool rotating, bool upsidedown, float xOffset, float yOffset);
    void removeAllRigidBodies();
    
    std::vector<DebugPoint> getDebugPoints();
    void showDebugPoint(DebugPoint p);
    void showDebugPoint(glm::vec3 position);
    void showDebugPoint(glm::vec3 position, glm::vec3 color);
    
private:
    std::vector<std::vector<RigidBody> > m_simulationStates;
    std::vector<DebugPoint> m_debugPoints;
    
    int m_activeRigidBody;
};