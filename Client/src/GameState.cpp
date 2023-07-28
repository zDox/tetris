#include "GameState.hpp"

GameState::GameState(std::shared_ptr<GameData> t_data) : data(t_data) {
}

void GameState::init(){
    initWindow();
    initVariables();
    game_logic.init();
    initUI();
}

void GameState::destroy(){
    data->gui.removeAllWidgets();
};


void GameState::initWindow(){  
    data->window->setFramerateLimit(GAME_FPS);
}

void GameState::initVariables(){
    drawing_grid.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            sf::RectangleShape rect(sf::Vector2f(SIDE_LENGTH, SIDE_LENGTH));
            rect.setFillColor(GRID_COLOR);
            rect.setPosition(k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_LEFT, i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_TOP);
            drawing_grid[i].push_back(rect);
        }
    }

}

void GameState::initUI(){
    points_label = tgui::Label::create("None");
    points_label->setPosition(400, 10);
    points_label->setTextSize(18);
    data->gui.add(points_label);

    paused_label = tgui::Label::create("Paused");
    paused_label->setOrigin(0.5f, 0.5f); // Set its anker to center of label 
    paused_label->setPosition(WIDTH/2.f, HEIGHT/2.f);
    paused_label->setTextSize(30);

    data->gui.add(paused_label);
}


 

void GameState::updateUI(){
    points_label->setText("Points: " + std::to_string(game_logic.getPoints()));
}

void GameState::handleKeyboard(){
    PlayerInput player_input;
    // Left
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !(hold_left)){
        hold_left = true;
        player_input.left = true;
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) hold_left = false;

    // Right
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !(hold_right)){
        hold_right = true;
        player_input.right = true;
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) hold_right = false;

    // Rotate
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !(hold_up)){
        hold_up = true;
        player_input.up = true;
    }
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) hold_up = false;

    // Down
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        player_input.down = true;
    }

    player_input.frame = frame_counter;
    data->network_manager.queuePlayerInput(player_input);
    game_logic.setPlayerInput(player_input);
}

void GameState::handleNextTetramino(){
    if(!game_logic.isNeedingNextTetramino()) return;
    try{
        game_logic.setNextTetramino(data->network_manager.getNextTetramino());
    }
    catch (std::out_of_range &e){
        // No next Tetramino available
        CORE_INFO("GameState - No next Tetramino available ");
    }
}

void GameState::drawGrid(){
    auto grid = game_logic.getGrid();
    for(int i=0; i < ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            drawing_grid[i][k].setFillColor(grid[i][k]);
            data->window->draw(drawing_grid[i][k]);
        }
    }
}

void GameState::drawOpponentGrids(){
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> oponent_grids = data->network_manager.getOpponentsGrid();
    int grid_pixel_width = ((SPACING_PER_RECT+SIDE_LENGTH)*COLUMNS);
    int max_grids_to_draw = std::min((int) oponent_grids.size()+1, (int)std::floor(WIDTH/grid_pixel_width));
    int count = 1; // Count of how many grids have been drawn
    for(auto[client_id, opponent_grid] : oponent_grids){
        if(count >= max_grids_to_draw) break;
        for(int i = 0; i < ROWS; i++){
            for(int k = 0; k < COLUMNS; k++){
                sf::RectangleShape rect(sf::Vector2(SIDE_LENGTH, SIDE_LENGTH));
                int pos_x = k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_LEFT + count*grid_pixel_width + count*SPACING_BETWEEN_GRIDS;
                int pos_y = i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_TOP;
                uint32_t color_uint = opponent_grid[i][k];
                uint8_t red = (color_uint >> 24) & 0xFF;
                uint8_t green = (color_uint >> 16) & 0xFF;
                uint8_t blue = (color_uint >> 8) & 0xFF;
                uint8_t alpha = color_uint & 0xFF;
                sf::Color color(red, green, blue, alpha);
                rect.setPosition(pos_x, pos_y);
                rect.setFillColor(color);
                data->window->draw(rect);
            }
        }
        count+=1;
    }
}

void GameState::handleInputs(){
    sf::Event event;
    while(data->window->pollEvent(event)){
        data->gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close();
        }
    }
};


void GameState::update(sf::Time dt){ 
    data->network_manager.update();
    if(data->network_manager.getConnectionStatus() == ConnectionStatus::DISCONNECTED or
       data->network_manager.getConnectionStatus() == ConnectionStatus::ERROR){
        data->state_manager.switchToState(std::make_shared<LoginState>(data));
    }

    if(data->network_manager.getRoundState() == RoundStateType::INGAME && !game_logic.isRunning()){
        game_logic.start();
        game_clock.restart();
    }

    if(data->network_manager.getGameID() != -1 && data->network_manager.getRoundState() == RoundStateType::INGAME){
        if(next_cycle <= game_clock.getElapsedTime()){
            handleKeyboard();
            handleNextTetramino();
            data->network_manager.update();
            game_logic.update(dt);
            CORE_TRACE("PERFORMANCE - Frame: {} game_clock: {}, next_cycle:{}", frame_counter, game_clock.getElapsedTime().asMilliseconds(), next_cycle.asMilliseconds());
            next_cycle = next_cycle + sf::seconds(1/TICK_RATE);
            frame_counter++;
        }
    }
    updateUI();
};


void GameState::draw(){
    data->window->clear(BACKGROUND_COLOR);
    
    drawGrid();
    drawOpponentGrids();

    data->gui.draw();
     
    data->window->display();
};
