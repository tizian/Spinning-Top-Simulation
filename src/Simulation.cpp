#include "Simulation.h"

#include "Collision.h"

using namespace std;

const int MAX_SIMULATION_SAVE_STATES = 1000;

Simulation::Simulation() {
    reset();
}

void Simulation::reset() {
    m_activeRigidBody = -1;
    vector<RigidBody> state = vector<RigidBody>();
    m_simulationStates = vector<vector<RigidBody> >();
    m_simulationStates.push_back(state);
}

vector<RigidBody> * Simulation::getCurrentState() {
    vector<RigidBody> * state = &m_simulationStates.back();
    return state;
}

vector<RigidBody> * Simulation::getLastState() {
    if (m_simulationStates.size() < 2) {
        return nullptr;
    }
    else {
        return &m_simulationStates.at(m_simulationStates.size() - 2);
    }
}

size_t Simulation::getNumberOfStates() {
    return m_simulationStates.size();
}

void Simulation::forwardStep(float dt) {
    m_debugPoints.clear();
    
    vector<RigidBody> newState = m_simulationStates.back();
    
    // update rigidbodies
    for (int i = 0; i < newState.size(); i++) {
        newState[i].update(dt);
        
        showDebugPoint(newState[i].getPosition());
    }
    
    // collision detection and response
    for (int i = 0; i < newState.size(); ++i) {
        for (int j = i+1; j < newState.size(); ++j) {
            
            std::vector<Contact> contacts = newState[i].intersectWith(newState[j]);
            if (contacts.size() > 0)
            {
//                printf("collisionPoints: %lu\n", contacts.size());
                Collision::collisionResponseBetween(newState[j], newState[i], contacts);
            }
        }
    }
    
    m_simulationStates.push_back(newState);
    
    if (m_simulationStates.size() > MAX_SIMULATION_SAVE_STATES) {
        m_simulationStates.erase(m_simulationStates.begin());
    }
}

void Simulation::backwardStep() {
    m_debugPoints.clear();
    
    if (m_simulationStates.size() > 1) {
        m_simulationStates.pop_back();
    }
    
    vector<RigidBody> * state = &m_simulationStates.back();
    m_activeRigidBody = -1;
    for (int i = 0; i < state->size(); i++) {
        if (state->at(i).isCurrentlyActive) {
            m_activeRigidBody = i;
        }
    }
}

RigidBody * Simulation::getActiveRigidBody() {
    if (m_activeRigidBody == -1) {
        return nullptr;
    }
    else {
        vector<RigidBody> * state = &m_simulationStates.back();
        return &state->at(m_activeRigidBody);
    }
}

void Simulation::removeActiveRigidBody() {
    if (m_activeRigidBody == -1) {
        return;
    }
    getActiveRigidBody()->isCurrentlyActive = false;
    getActiveRigidBody()->setMaterial(Assets::getWhiteMaterial());
    
    vector<RigidBody> * state = &m_simulationStates.back();
    state->erase(state->begin() + m_activeRigidBody);
    m_activeRigidBody++;
    if (m_activeRigidBody > state->size() - 1) {
        m_activeRigidBody = 0;
    }
    if (state->size() == 0) {
        m_activeRigidBody = -1;
    }
    
    if (m_activeRigidBody == -1) {
        return;
    }
    getActiveRigidBody()->isCurrentlyActive = true;
    getActiveRigidBody()->setMaterial(Assets::getSlightlyGreenMaterial());
}

void Simulation::removeAllRigidBodies() {
    vector<RigidBody> * state = &m_simulationStates.back();
    state->erase(state->begin(), state->end());
    m_activeRigidBody = -1;
}

void Simulation::toggleActiveRigidBody() {
    vector<RigidBody> * state = &m_simulationStates.back();
    
    if (m_activeRigidBody < 0) {
        return;
    }
    else {
        getActiveRigidBody()->isCurrentlyActive = false;
        getActiveRigidBody()->setMaterial(Assets::getWhiteMaterial());
        if (m_activeRigidBody < state->size() - 1) {
            m_activeRigidBody++;
        }
        else {
            m_activeRigidBody = 0;
        }
        getActiveRigidBody()->isCurrentlyActive = true;
        getActiveRigidBody()->setMaterial(Assets::getSlightlyGreenMaterial());
    }
}

void Simulation::addRigidBody(int type, bool rotating, bool upsidedown) {
    RigidBody rb;
    RigidBodyFactory::resetSpinningTop(rb, type, rotating, upsidedown);
    
    vector<RigidBody> * state = &m_simulationStates.back();
    state->push_back(rb);
    
    if (m_activeRigidBody == -1) {
        m_activeRigidBody = 0;
        getActiveRigidBody()->isCurrentlyActive = true;
        getActiveRigidBody()->setMaterial(Assets::getSlightlyGreenMaterial());
    }
}

std::vector<DebugPoint> Simulation::getDebugPoints() {
    return m_debugPoints;
}

void Simulation::showDebugPoint(DebugPoint p) {
    m_debugPoints.push_back(p);
}

void Simulation::showDebugPoint(glm::vec3 position) {
    DebugPoint p;
    p.color = glm::vec3(0.8, 0.0, 0.0);
    p.position = position;
    showDebugPoint(p);
}

void Simulation::showDebugPoint(glm::vec3 position, glm::vec3 color) {
    DebugPoint p;
    p.color = color;
    p.position = position;
    showDebugPoint(p);
}
