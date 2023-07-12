#ifndef STATE_HPP
#define STATE_HPP


#include "SFML/System.hpp"

class State
{
    friend class StateManager;

public:

    virtual void init() = 0;
    virtual void destroy() = 0;


    virtual void handleInputs() = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void draw() = 0;
};
#endif
