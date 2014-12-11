#include "Simulation.h"

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
    
    vector<RigidBody> newState = vector<RigidBody>();
    
    vector<RigidBody> * state = &m_simulationStates.back();
    
    for (int i = 0; i < m_simulationStates.back().size(); i++) {
        newState.push_back(state->at(i));
    }
    
    // collision detection and response
    for (int i = 0; i < newState.size(); ++i) {
        for (int j = i+1; j < newState.size(); ++j) {
            std::vector<Contact> intersections = newState[i].intersectWith(newState[j]);
            if (intersections.size() > 0)
            {
                printf("collision with: %lu points\n", intersections.size());
                //newState[i].setDebugPoint(intersections[0]);
                //newState[j].setDebugPoint(intersections[0]);
                
                glm::vec3 direction = (newState[i].getPosition() - newState[j].getPosition());
                direction *= (1.f/glm::length(direction));
                newState[i].addImpulse(1.f * direction); // hardcoded hack
                newState[j].addImpulse(-1.5f * direction); // hardcoded hack
            }
        }
    }
    
    // update rigidbodies
    for (int i = 0; i < newState.size(); i++) {
        newState[i].update(dt);

        showDebugPoint(newState[i].getPosition());
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
    getActiveRigidBody()->setMaterial(&Assets::whiteMaterial);
    
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
    getActiveRigidBody()->setMaterial(&Assets::slightlyGreenMaterial);
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
        getActiveRigidBody()->setMaterial(&Assets::whiteMaterial);
        if (m_activeRigidBody < state->size() - 1) {
            m_activeRigidBody++;
        }
        else {
            m_activeRigidBody = 0;
        }
        getActiveRigidBody()->isCurrentlyActive = true;
        getActiveRigidBody()->setMaterial(&Assets::slightlyGreenMaterial);
    }
}

void Simulation::addRigidBody(int type) {
    RigidBody rb;
    RigidBodyFactory::resetSpinningTop(rb, type);
    
    vector<RigidBody> * state = &m_simulationStates.back();
    state->push_back(rb);
    
    if (m_activeRigidBody == -1) {
        m_activeRigidBody = 0;
        getActiveRigidBody()->isCurrentlyActive = true;
        getActiveRigidBody()->setMaterial(&Assets::slightlyGreenMaterial);
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

void Simulation::showDebugPoint(glm::vec3 position, glm::vec3 color) {
    DebugPoint p;
    p.color = color;
    p.position = position;
    showDebugPoint(p);
}
