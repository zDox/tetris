#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include <vector>
#include <algorithm>
#include <iostream>
#include "utils.hpp"

#include <SFML/Graphics.hpp>

enum TETRAMINO_TYPE{
    STRAIGHT=0,
    SQUARE,
    T_SHAPE,
    L_SHAPE,
    J_SHAPE,
    S_SHAPE,
    Z_SHAPE,
    AMOUNT,
};

class Tetramino{
private:
    TETRAMINO_TYPE type;
    std::vector<std::vector<bool>> form;
    int rotation;
    int x, y;
    sf::Color color;
public:
    Tetramino(TETRAMINO_TYPE t_type, sf::Color t_color);
    
    // Accessor
    std::vector<std::vector<bool>> getForm();

    void setPosition(int t_x, int t_y);
    int getX();
    int getY();

    void setColor(sf::Color);
    sf::Color getColor();

    // Functions
    void rotate(bool clockwise);
    void move(int dx, int dy);

    void print();

};
#endif
