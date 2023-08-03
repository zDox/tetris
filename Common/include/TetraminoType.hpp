#ifndef TETRAMINO_TYPE
#define TETRAMINO_TYPE

#include <SFML/Graphics/Color.hpp>

enum class TetraminoType{
    STRAIGHT=0,
    SQUARE,
    T_SHAPE,
    L_SHAPE,
    J_SHAPE,
    S_SHAPE,
    Z_SHAPE,
    AMOUNT,
    MIN=STRAIGHT,
    MAX=Z_SHAPE,
};

inline bool isValidTetraminoType(TetraminoType test){
    return (TetraminoType::MIN <= test && test <= TetraminoType::MAX);
}

inline sf::Color generateColor(TetraminoType type){
    switch(type){
        case TetraminoType::STRAIGHT:
            return sf::Color(67,249,249); // Cyan
        case TetraminoType::SQUARE:
            return sf::Color(255,255,68); // Yellow
        case TetraminoType::T_SHAPE:
            return sf::Color(255,68,255); // Purple
        case TetraminoType::L_SHAPE:
            return sf::Color(255,136,0); // Orange
        case TetraminoType::J_SHAPE:
            return sf::Color(6,4,212); // Blue
        case TetraminoType::S_SHAPE:
            return sf::Color(1,239,0); // Green
        case TetraminoType::Z_SHAPE:
            return sf::Color(228,4,5); // Red
        default:
            return sf::Color::Black;
    }
}

#endif
