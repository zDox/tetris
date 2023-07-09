#include "game.hpp"
#include "tetramino.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

const float Game::SIDE_LENGTH = static_cast<float>(HEIGHT-(SPACING_TOP + SPACING_BOTTOM)) / static_cast<float>(ROWS) - SPACING_PER_RECT;


Game::Game(){
    initVariables();
    initWindow();
}

void Game::initVariables(){
    grid.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            sf::RectangleShape rect(sf::Vector2f(SIDE_LENGTH, SIDE_LENGTH));
            grid[i].push_back(rect);
        }
    }
}

void Game::initWindow(){
    videomode.height = HEIGHT;
    videomode.width = WIDTH;

    window = std::make_shared<sf::RenderWindow>(videomode, "Snake Game", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(FPS);
}

void Game::handleEvents(){
    while(window->pollEvent(event)){
        if(event.type == sf::Event::Closed){
            running = false;
        }
    }
};

void Game::spawnTetramino(){
    TETRAMINO_TYPE selection = static_cast<TETRAMINO_TYPE>(std::rand() % (TETRAMINO_TYPE::AMOUNT-1));
    tetra = std::make_shared<Tetramino>(selection);
    tetra->setPosition(7-tetra->getForm().size(), 0);
}

void Game::resetGrid(){
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i][k].setFillColor(sf::Color::White);
        }
    }
}

void Game::putTetraOnGrid(){
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
                                     
            grid[tetra->getY()+i][tetra->getX()+k].setFillColor(sf::Color::Blue);
        }
    }
};

void Game::updateTetra(){
    if(!tetra) spawnTetramino();
    sf::Time max_move_time = sf::seconds((1.f/SPEED));
    if(tetra_move_timer.getElapsedTime() >= max_move_time){
        tetra->move(0, 1);
        tetra_move_timer.restart();
    }
}

void Game::update(){
    resetGrid();
    updateTetra();
    putTetraOnGrid();
};

void Game::drawGrid(){
    for(int i=0; i < ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i][k].setPosition(k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_TOP, i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_LEFT);
            window->draw(grid[i][k]);
        }
    }
}



void Game::render(){
    window->clear(sf::Color::Black);
    
    drawGrid();
     
    window->display();
};

void Game::run(){
    handleEvents();
    update();
    render();
}

void Game::close(){
    window->close();
}

bool Game::isRunning(){
    return running;
}
