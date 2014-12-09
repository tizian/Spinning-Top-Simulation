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
    vector<RigidBody> newState = vector<RigidBody>();
    
    vector<RigidBody> * state = &m_simulationStates.back();
    
    for (int i = 0; i < m_simulationStates.back().size(); i++) {
        newState.push_back(state->at(i));
    }
    
    for (int i = 0; i < newState.size(); i++) {
        newState[i].update(dt);
    }
    
    m_simulationStates.push_back(newState);
    
    if (m_simulationStates.size() > MAX_SIMULATION_SAVE_STATES) {
        m_simulationStates.erase(m_simulationStates.begin());
    }
}

void Simulation::backwardStep() {
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
