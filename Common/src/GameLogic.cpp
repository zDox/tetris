#include "GameLogic.hpp"

GameLogic::GameLogic(){
};

void GameLogic::initVariables(){
    grid.resize(ROWS);
    stationaries.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            sf::RectangleShape rect(sf::Vector2f(SIDE_LENGTH, SIDE_LENGTH));
            grid[i].push_back(rect);
            sf::RectangleShape rect2 = rect;
            stationaries[i].push_back(rect2);
        }
    }
    tetra_move_timer.restart();
}

void GameLogic::spawnTetramino(){
    TetraminoType selection = static_cast<TetraminoType>(std::rand() % (TetraminoType::AMOUNT-1));
    sf::Color random_color = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256, 255);
    tetra = std::make_shared<Tetramino>(selection, random_color);
    tetra->setPosition(7-tetra->getForm().size(), 0);

    if(checkLoss()){
        handleLoss();
    }
}

void GameLogic::resetGrid(){
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i][k] = GRID_COLOR;
        }
    }
}

void GameLogic::putTetraOnGrid(){
    if(!tetra) return;
    std::vector<std::vector<bool>> form = tetra->getForm();
    for(int i=0; i<form.size(); i++){
        for(int k=0; k<form.size(); k++){
            if(!form[i][k]) continue;
            
            // Dont if out of grid in X Direction
            if(!(0 <= tetra->getX() + k && tetra->getX() + k < COLUMNS)) 
                continue;
            // Dont if out of grid in Y Direction
            if(!(0 <= tetra->getY()+i && tetra->getY() + i < ROWS)) 
                continue;             
                                     
            grid[tetra->getY()+i][tetra->getX()+k] = tetra->getColor();
        }
    }
};

bool GameLogic::checkCollisions(int dx, int dy, bool clockwise){
    Tetramino temp = *tetra; // Make copy of tetra to check new move
    if(clockwise) temp.rotate(clockwise);
    else if(dx != 0) temp.move(dx,0);
    else if(dy != 0) temp.move(0, dy);

    std::vector<std::vector<bool>> form = temp.getForm();

    for(int i=0; i<form.size(); i++){
        for(int k=0; k<form[0].size(); k++){
            if(!form[i][k]) continue;
            // Out of grid, left or right check
            if((!(0<=temp.getX()+ k && temp.getX() + k < COLUMNS)) or temp.getY()+i >= ROWS){
                return true;
            }
            if(grid[temp.getY()+i][temp.getX()+k] != GRID_COLOR){
                return true;
            }
        }
    }
    return false;
};

void GameLogic::lockTetra(){
    std::vector<std::vector<bool>> form = tetra->getForm();
    for(int i=0; i<form.size(); i++){
        for(int k=0; k<form.size(); k++){
            if(!form[i][k]) continue;
            
            // Dont if out of grid in X Direction
            if(!(0 <= tetra->getX() + k && tetra->getX() + k < COLUMNS)) 
                continue;
            // Dont if out of grid in Y Direction
            if(!(0 <= tetra->getY()+i && tetra->getY() + i < ROWS)) 
                continue;             
                                     
            stationaries[tetra->getY()+i][tetra->getX()+k] = sf::Color::Magenta;
        }
    }
    tetra = nullptr;
};

bool GameLogic::checkLoss(){
    std::vector<std::vector<bool>> form = tetra->getForm();
    for(int i=0;i<form.size(); i++){
        for(int k=0; k<form.size(); k++){
            if(!form[i][k]) continue;
            if(stationaries[tetra->getY() + k][tetra->getX() + i] != GRID_COLOR) return true;
        }
    }
    return false;
};

void GameLogic::handleLoss(){
    finished = true;
    std::cout << "Points: " << std::to_string(points) << "\n";
};

void GameLogic::putStationariesOnGrid(){
    for(int i=0; i < ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            if(stationaries[i][k].getFillColor() != GRID_COLOR){
                grid[i][k] = stationaries[i][k];
            }
        }
    }
};

void GameLogic::updateTetra(PlayerCommandType command_type){
    sf::Time max_move_time = sf::seconds(10.f/SPEED);
    if(tetra_move_timer.getElapsedTime() >= max_move_time){
        if(checkCollisions(0, 1, false)){
            lockTetra();
            return;
        }
        tetra->move(0, 1);
        tetra_move_timer.restart();
    }

    // Move left or right
    switch(command_type){
        case PlayerCommandType::MOVE_LEFT:
            if(checkCollisions(-1, 0, false)) return;
            tetra->move(-1, 0);
            break;

        case PlayerCommandType::MOVE_RIGHT: 
            if(checkCollisions(1, 0, false)) return;
            tetra->move(1, 0);
            break;

        case PlayerCommandType::ROTATE_CLOCKWISE:
            if(checkCollisions(0, 0, true)) return;
            tetra->rotate(true);
            break;
    }
}

void GameLogic::checkPoint(){
    for(int i=0; i<ROWS; i++){
        bool completed = true;
        for(int k=0; k<COLUMNS; k++){
            if(stationaries[i][k] == GRID_COLOR){
                completed = false;
                break;
            }
        }
        
        if(completed){
            points++;
            // First row becomes empty
            for(int k=0; k<COLUMNS; k++){
                stationaries[0][k] = GRID_COLOR;
            }
            // Move all rows on down up to i
            for(int j =i; 0 < j; j--){
                for(int k=0; k<COLUMNS; k++){
                    stationaries[j][k] = stationaries[j-1][k];
                }
            }
        }
    }
}
