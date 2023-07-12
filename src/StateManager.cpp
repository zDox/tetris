#include "StateManager.hpp"

StateManager::StateManager(){
};
void StateManager::switchToState(std::shared_ptr<State> state){
    newState = state;
};

void StateManager::processStateChanges(){
    // Only change current_state if new one is supplied
    if(newState == nullptr) return;
    if(current_state){
        current_state->destroy();
    }
    current_state = newState;
    current_state->init();
    newState = nullptr;
}

void StateManager::handleInputs(){
    current_state->handleInputs();
}

void StateManager::updateState(sf::Time dt){
    // Update states
    current_state->update(dt);
};

void StateManager::drawState(){
    current_state->draw();
};

void StateManager::destroyState(){
    if(current_state){
        current_state->destroy();
    }
}

void StateManager::clearAll(){
    current_state = nullptr;
    newState = nullptr;
};

