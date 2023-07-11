#include "game.hpp"
#include <memory>

const float Game::SIDE_LENGTH = static_cast<float>(HEIGHT-(SPACING_TOP + SPACING_BOTTOM)) / static_cast<float>(ROWS) - SPACING_PER_RECT;


Game::Game(){
    initVariables();
    initWindow();
    initUI();
}

void Game::initVariables(){
    std::srand(std::time(0));
    grid.resize(ROWS);
    stationaries.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            sf::RectangleShape rect(sf::Vector2f(SIDE_LENGTH, SIDE_LENGTH));
            rect.setFillColor(GRID_COLOR);
            grid[i].push_back(rect);
            sf::RectangleShape rect2 = rect;
            stationaries[i].push_back(rect2);
        }
    }
    tetra_move_timer.restart();
}

void Game::initWindow(){
    videomode.height = HEIGHT;
    videomode.width = WIDTH;

    window = std::make_shared<sf::RenderWindow>(videomode, "Snake Game", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(FPS);

    
}

void Game::initUI(){
    gui.setWindow(*window);

    points_label = tgui::Label::create("None");
    points_label->setPosition(400, 10);
    points_label->setTextSize(18);
    gui.add(points_label);

    paused_label = tgui::Label::create("Paused");
    paused_label->setOrigin(0.5f, 0.5f); // Set its anker to center of label 
    paused_label->setPosition(WIDTH/2.f, HEIGHT/2.f);
    paused_label->setTextSize(30);

    gui.add(paused_label);
}


void Game::handleEvents(){
    while(window->pollEvent(event)){
        gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            running = false;
        }
        if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Space){
                paused = !(paused);
            }
        }
    }
};

void Game::spawnTetramino(){
    TETRAMINO_TYPE selection = static_cast<TETRAMINO_TYPE>(std::rand() % (TETRAMINO_TYPE::AMOUNT-1));
    sf::Color random_color = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256, 255);
    tetra = std::make_shared<Tetramino>(selection, random_color);
    tetra->setPosition(7-tetra->getForm().size(), 0);

    if(checkLoss()){
        handleLoss();
    }
}

void Game::resetGrid(){
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i][k].setFillColor(GRID_COLOR);
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
                                     
            grid[tetra->getY()+i][tetra->getX()+k].setFillColor(tetra->getColor());
        }
    }
};

void Game::putStationariesOnGrid(){
   for(int i=0; i < ROWS; i++){
       for(int k=0; k<COLUMNS; k++){
           if(stationaries[i][k].getFillColor() != GRID_COLOR){
               grid[i][k] = stationaries[i][k];
            }
        }
    }
};

bool Game::checkCollisions(int dx, int dy, bool clockwise){
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
            if(grid[temp.getY()+i][temp.getX()+k].getFillColor() != GRID_COLOR){
                return true;
            }
        }
    }
    return false;
};

void Game::lockTetra(){
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
                                     
            stationaries[tetra->getY()+i][tetra->getX()+k].setFillColor(sf::Color::Magenta);
        }
    }
    tetra = nullptr;
};

void Game::updateTetra(){
    if(!tetra) spawnTetramino();
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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !(hold_left)){
        hold_left = true;
        if(checkCollisions(-1, 0, false)) return;
        tetra->move(-1, 0);
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) hold_left = false;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !(hold_right)){
        hold_right = true;
        if(checkCollisions(1, 0, false)) return;
        tetra->move(1, 0);
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) hold_right = false;

    // Rotate
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !(hold_up)){
        hold_up = true;
        if(checkCollisions(0, 0, true)) return;
        tetra->rotate(true);
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) hold_up = false;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        if(checkCollisions(0, 1, false)){
            lockTetra();
            return;
        }
        tetra->move(0, 1);
    }
}

void Game::checkPoint(){
    for(int i=0; i<ROWS; i++){
        bool completed = true;
        for(int k=0; k<COLUMNS; k++){
            if(stationaries[i][k].getFillColor() == GRID_COLOR){
                completed = false;
                break;
            }
        }
        
        if(completed){
            points++;
            // First row becomes empty
            for(int k=0; k<COLUMNS; k++){
                stationaries[0][k].setFillColor(GRID_COLOR);
            }
            // Move all rows on down up to i
            for(int j =i; 0 < j; j--){
                for(int k=0; k<COLUMNS; k++){
                    stationaries[j][k].setFillColor(stationaries[j-1][k].getFillColor());
                }
            }
        }
    }
}

bool Game::checkLoss(){
    std::vector<std::vector<bool>> form = tetra->getForm();
    for(int i=0;i<form.size(); i++){
        for(int k=0; k<form.size(); k++){
            if(!form[i][k]) continue;
            if(stationaries[tetra->getY() + k][tetra->getX() + i].getFillColor() != GRID_COLOR) return true;
        }
    }
    return false;
};
            
void Game::handleLoss(){
    finished = true;
    std::cout << "Points: " << std::to_string(points) << "\n";
};

void Game::updateUI(){
    paused_label->setVisible(paused);
    points_label->setText("Points: " + std::to_string(points));
}

void Game::update(){ 
    if(!paused and !finished){
        resetGrid();
        putStationariesOnGrid();
        updateTetra();
        checkPoint();
        putTetraOnGrid();
    }
    updateUI();
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
    window->clear(BACKGROUND_COLOR);
    
    drawGrid();
    
    gui.draw();
     
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
