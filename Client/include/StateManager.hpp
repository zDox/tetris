#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include <memory>
#include <deque>
#include <set>

#include "State.hpp"

class StateManager{

    std::shared_ptr<State> current_state; 
    std::shared_ptr<State> newState = nullptr; // State that has to be added
public:
    StateManager();
    // Puts state in newState to later be switchted to 
    void switchToState(std::shared_ptr<State> state);
    void processStateChanges();
    bool isSwitching();

    void handleInputs();
    void updateState(sf::Time dt);
    void drawState();
    void destroyState();

    void clearAll();
};

#endif
