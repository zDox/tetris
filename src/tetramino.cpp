#include "tetramino.hpp"


Tetramino::Tetramino(TETRAMINO_TYPE t_type, sf::Color t_color){
    type = t_type;
    color = t_color;
    switch(type){
        case TETRAMINO_TYPE::STRAIGHT:
            form = {{0,0,0,0},
                    {1,1,1,1},
                    {0,0,0,0},
                    {0,0,0,0}};
            break;
        case TETRAMINO_TYPE::SQUARE:
            form = {{1,1},
                    {1,1}};
            break;
        case TETRAMINO_TYPE::T_SHAPE:
            form = {{0,1,0},
                    {1,1,1},
                    {0,0,0}};
            break;
        case TETRAMINO_TYPE::L_SHAPE:
            form = {{0,0,1},
                    {1,1,1},
                    {0,0,0}};
            break;
        case TETRAMINO_TYPE::J_SHAPE:
            form = {{1,0,0},
                    {1,1,1},
                    {0,0,0}};
            break;
        case TETRAMINO_TYPE::S_SHAPE:
            form = {{0,1,1},
                    {1,1,0},
                    {0,0,0}};
            break;
        case TETRAMINO_TYPE::Z_SHAPE:
            form = {{1,1,0},
                    {0,1,1},
                    {0,0,0}};
            break;
        case TETRAMINO_TYPE::AMOUNT:
            break;
    }
    rotation = 0;
};

std::vector<std::vector<bool>> Tetramino::getForm(){
    return form;
};

void Tetramino::setPosition(int t_x, int t_y){
    x = t_x;
    y = t_y;
};

void Tetramino::move(int dx, int dy){
    x += dx;
    y += dy;
};

int Tetramino::getX(){
    return x;
};

int Tetramino::getY(){
    return y;
};

void Tetramino::setColor(sf::Color t_color){
    color = t_color;
}

sf::Color Tetramino::getColor(){
    return color;
}

void Tetramino::rotate(bool clockwise){
    if(clockwise){
        transpose(form);
        reverse_rows(form);
    }
    else{
        reverse_rows(form);
        transpose(form);
    }
};

void Tetramino::print(){
    for(int i=0; i<form.size(); i++){
        for(int k=0; k< form[0].size(); k++){
            std::cout << std::to_string(form[i][k]);
            if(k != form[0].size()-1){
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
};
