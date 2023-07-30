#include "GameLogic.hpp"
GameLogic::GameLogic(){
};

void GameLogic::init(){
    initVariables();
}

void GameLogic::start(){
    CORE_INFO("GameLogic - Starting GameLogic");
    running = true;
}

bool GameLogic::isRunning(){
    return running;
}
void GameLogic::setNextTetramino(TetraminoType tetramino){
    CORE_TRACE("GameLogic - Next Tetramino: {}", (int)tetramino);
    next_tetramino = std::make_shared<TetraminoType>();
    *next_tetramino = tetramino;
}

bool GameLogic::isNeedingNextTetramino(){
    return next_tetramino == nullptr;
}

void GameLogic::setPlayerInput(PlayerInput t_player_input){
    player_input = std::make_shared<PlayerInput>();
    *player_input = t_player_input;
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
}

void GameLogic::spawnTetramino(){
    CORE_TRACE("GameLogic - Spawning new Tetramino");
    if(!next_tetramino.get()) {
        CORE_WARN("GameLogic - No new TetraminoType available");
        return;
    }
    tetra = std::make_shared<Tetramino>(*next_tetramino, sf::Color::Yellow);
    tetra->setPosition(7-tetra->getForm().size(), 0);

    next_tetramino = nullptr;

    if(checkLoss()){
        handleLoss();
    }
}

void GameLogic::updateTime(sf::Time dt){
    game_time += dt;
    CORE_TRACE("GameLogic - GameTime: {} us", game_time.asMicroseconds());
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
    running = false;
    finished = true;
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
    if(next_move_time <= game_time){
        if(checkCollisions(0, 1, false)){
            lockTetra();
            return;
        }
        tetra->move(0, 1);
        next_move_time+=max_move_time;
    }

    if(!player_input) return;

    if(player_input->down){
        if(checkCollisions(0, 1, false)){
            lockTetra();
        }
        tetra->move(0, 1);
    }
        
    if(player_input->left){
        if(checkCollisions(-1, 0, false)) return;
        tetra->move(-1, 0);
    }

    if(player_input->right){
        if(checkCollisions(1, 0, false)) return;
        tetra->move(1, 0);
    }

    if(player_input->up){
        if(checkCollisions(0, 0, true)) return;
        tetra->rotate(true);
    }
    player_input = nullptr;
}


void GameLogic::update(sf::Time dt){
    if(!running) return;
    updateTime(dt);
    resetGrid();
    putStationariesOnGrid();
    updateTetra();
    checkPoint();
    putTetraOnGrid();
}
