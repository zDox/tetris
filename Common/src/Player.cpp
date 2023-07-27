#include "Player.hpp"

bool comp(const PlayerInput &l, const long &frame){
    return l.frame < frame;
}
