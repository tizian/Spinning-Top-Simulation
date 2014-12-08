#pragma once

#include "RigidBody.h"
#include "RigidBodyFactory.h"

class Simulation {
public:
    Simulation();
    
    void reset();
    
    std::vector<RigidBody> * getCurrentState();
    std::vector<RigidBody> * getLastState();
    
    size_t getNumberOfStates();

    void forwardStep(float dt);
    void backwardStep();
    
    RigidBody * getActiveRigidBody();
    void removeActiveRigidBody();
    void toggleActiveRigidBody();
    void addRigidBody(int type);
    void removeAllRigidBodies();
    
private:
    std::vector<std::vector<RigidBody> > m_simulationStates;
    
    int m_activeRigidBody;
};