#include "GameLogic.hpp"

GameLogic::GameLogic(){
};

void GameLogic::init(){
    initVariables();
}

void GameLogic::start(){
    tetra_move_timer.restart();
    next_move_time = sf::seconds(0);
    running = true;
}

bool GameLogic::isRunning(){
    return running;
}
void GameLogic::setNextTetramino(TetraminoType tetramino){
    std::cout << "Next Tetramino: " << (int)tetramino << "\n";
    next_tetramino = std::make_shared<TetraminoType>();
    *next_tetramino = tetramino;
}

bool GameLogic::isNeedingNextTetramino(){
    return next_tetramino == nullptr;
}

void GameLogic::setPlayerCommand(PlayerCommandType t_player_command){
    player_command = t_player_command;
}

std::vector<std::vector<sf::Color>> GameLogic::getGrid(){
    return grid;
}

int GameLogic::getPoints(){
    return points;
}

void GameLogic::initVariables(){
    grid.resize(ROWS);
    stationaries.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i].push_back(GRID_COLOR);
            stationaries[i].push_back(GRID_COLOR);
        }
    }
    tetra_move_timer.restart();
}

void GameLogic::spawnTetramino(){
    std::cout << "Spawning new Tetramino\n";
    if(!next_tetramino.get()) {
        std::cout << "No new TetraminoType available\n";
        return;
    }
    tetra = std::make_shared<Tetramino>(*next_tetramino, sf::Color::Yellow);
    tetra->setPosition(7-tetra->getForm().size(), 0);

    next_tetramino = nullptr;

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
    if(!tetra.get()) return;
    std::vector<std::vector<bool>> form = tetra->getForm();
    for(std::vector<std::vector<bool>>::size_type i=0; i<form.size(); i++){
        for(std::vector<bool>::size_type k=0; k<form.size(); k++){
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

void GameLogic::putStationariesOnGrid(){
    for(int i=0; i < ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            if(stationaries[i][k] != GRID_COLOR){
                grid[i][k] = stationaries[i][k];
            }
        }
    }
};

bool GameLogic::checkCollisions(int dx, int dy, bool clockwise){
    Tetramino temp = *tetra; // Make copy of tetra to check new move
    if(clockwise) temp.rotate(clockwise);
    else if(dx != 0) temp.move(dx,0);
    else if(dy != 0) temp.move(0, dy);

    std::vector<std::vector<bool>> form = temp.getForm();

    for(std::vector<std::vector<bool>>::size_type i=0; i<form.size(); i++){
        for(std::vector<bool>::size_type k=0; k<form[0].size(); k++){
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
    for(std::vector<std::vector<bool>>::size_type i=0; i<form.size(); i++){
        for(std::vector<bool>::size_type k=0; k<form.size(); k++){
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
    for(std::vector<std::vector<bool>>::size_type i=0;i<form.size(); i++){
        for(std::vector<bool>::size_type k=0; k<form.size(); k++){
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

void GameLogic::updateTetra(){
    if(!tetra.get()){
        spawnTetramino();
        if(!tetra.get()) return;
    }

    sf::Time max_move_time = sf::seconds(10.f/SPEED);
    if(next_move_time <= tetra_move_timer.getElapsedTime()){
        if(checkCollisions(0, 1, false)){
            lockTetra();
            return;
        }
        tetra->move(0, 1);
        next_move_time+=max_move_time;
    }

    // Move left or right
    switch(player_command){
        case PlayerCommandType::MOVE_DOWN:
            if(checkCollisions(0, 1, false)){
                lockTetra();
            }
            tetra->move(0, 1);
            return;
        
        tetra->move(0, 1);
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

        default:
            break;
    }
    player_command = PlayerCommandType::NONE;
}

void GameLogic::update(sf::Time dt){
    if(!running) return;
    resetGrid();
    putStationariesOnGrid();
    updateTetra();
    checkPoint();
    putTetraOnGrid();
}
