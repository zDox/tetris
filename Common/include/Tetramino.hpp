#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include <vector>
#include <algorithm>
#include <iostream>

// 3rd Party
#include <SFML/Graphics.hpp>

#include "Utils.hpp"
#include "TetraminoType.hpp"

class Tetramino{
private:
    TetraminoType type;
    std::vector<std::vector<bool>> form;
    int rotation;
    int x, y;
    sf::Color color;
public:
    Tetramino(TetraminoType t_type, sf::Color t_color);
    
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
