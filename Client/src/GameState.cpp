#include "GameState.hpp"

GameState::GameState(std::shared_ptr<GameData> t_data) : data(t_data) {
    client_id = data->network_manager.getClientID();
}

void GameState::init(){
    initWindow();
    initVariables();
    initHandlers();
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
    std::shared_ptr<ClientPlayer> c_player = std::make_shared<ClientPlayer>();
    c_player->drawing_grid.resize(ROWS);
    c_player->player.client_id = client_id;
    players.emplace(client_id, c_player);
    
    initPlayerDrawingGrid(client_id);
    initPlayerUI(client_id);

    tetramino_queue = {};
}

void GameState::initHandlers(){
    data->network_manager.registerMessageHandler(
            MessageType::GRID, 
            std::bind(&GameState::handleGridMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::ROUNDSTATECHANGE, 
            std::bind(&GameState::handleRoundStateChangeMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::TETRAMINO_PLACEMENT,
            std::bind(&GameState::handleTetraminoPlacementMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_SCORE, 
            std::bind(&GameState::handlePlayerScoreMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_JOIN, 
            std::bind(&GameState::handlePlayerJoinMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_LEAVE, 
            std::bind(&GameState::handlePlayerLeaveMessage, this, std::placeholders::_1));
    CORE_DEBUG("GameState - registration of message handler completed");
}

void GameState::initUI(){
}


void GameState::initPlayerDrawingGrid(uint64_t p_client_id){
    if(!players.contains(p_client_id)) return;
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];
    c_player->drawing_grid.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            std::shared_ptr<sf::RectangleShape> rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(SIDE_LENGTH, SIDE_LENGTH));
            rect->setFillColor(GRID_COLOR);
            rect->setPosition(k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_LEFT, i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_TOP);
            c_player->drawing_grid[i].push_back(rect);
        }
    }
 }

void GameState::initPlayerUI(uint64_t p_client_id){
    if(!players.contains(p_client_id)) return;
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];
    c_player->main_label = tgui::Label::create();
    c_player->stats_label = tgui::Label::create();

    c_player->main_label->setVisible(false);
    c_player->main_label->setTextSize(20);

    c_player->stats_label->setVisible(false);
    c_player->stats_label->setTextSize(14);

    data->gui.add(c_player->main_label);
    data->gui.add(c_player->stats_label);
}

void GameState::updateUI(){
}

void GameState::updatePlayerUI(uint64_t p_client_id){
    if(!players.contains(p_client_id)) return;
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];

    tgui::String stats_text = 
        "points: " + std::to_string(c_player->player.points) + 
        "\nposition: " + std::to_string(c_player->player.position);
    c_player->stats_label->setText(stats_text);

    tgui::String main_text;
    switch(roundstate){
        case RoundStateType::LOBBY:
            c_player->stats_label->setVisible(false);
            c_player->main_label->setVisible(true);
            main_text = "Waiting for start...";
            break;
        case RoundStateType::INGAME:
            c_player->stats_label->setVisible(true);
            c_player->main_label->setVisible(false);
            break;

        case RoundStateType::END:
            c_player->stats_label->setVisible(false);
            c_player->main_label->setVisible(true);
            main_text = "Place " + std::to_string(c_player->player.position) + ".";
            break;
        default:
            break;
    }

    c_player->main_label->setText(main_text);
}

void GameState::updatePlayerUIs(){
    for(auto [p_client_id, player] : players){
        updatePlayerUI(p_client_id);
    }
}

void GameState::handleKeyboard(){
    PlayerInput player_input;
    if(data->window->hasFocus()){
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
    }

    player_input.frame = frame_counter;
    data->network_manager.queuePlayerInput(player_input);
    game_logic.setPlayerInput(player_input);
}

void GameState::handleNextTetramino(){
    if(!game_logic.isNeedingNextTetramino()) return;
    if(tetramino_queue.empty()) {
        return;
    }
    TetraminoType next = tetramino_queue.front();
    tetramino_queue.pop();
    game_logic.setNextTetramino(next);
}

void GameState::drawPlayer(uint64_t p_client_id, int offset_x, int offset_y, float scale){
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];
    CORE_DEBUG("GameState - drawPlayer - off_x: {}, off_y: {}", offset_x, offset_y);

    // Draw the grid 
    for(int i = 0; i < ROWS; i++){
        for(int k = 0; k < COLUMNS; k++){
            int pos_x = k*SIDE_LENGTH*scale + k*SPACING_PER_RECT*scale + offset_x;
            int pos_y = i*SIDE_LENGTH*scale + i*SPACING_PER_RECT*scale + offset_y;
            uint32_t color_uint = c_player->player.grid[i][k];
            uint8_t red = (color_uint >> 24) & 0xFF;
            uint8_t green = (color_uint >> 16) & 0xFF;
            uint8_t blue = (color_uint >> 8) & 0xFF;
            uint8_t alpha = color_uint & 0xFF;
            sf::Color color(red, green, blue, alpha);
            std::shared_ptr<sf::RectangleShape> rect = players[p_client_id]->drawing_grid[i][k];
            rect->setPosition(pos_x, pos_y);
            rect->setFillColor(color);
            data->window->draw(*rect);
        }
    }

    // Draw UI Elements
    c_player->stats_label->setPosition(offset_x, offset_y);
    c_player->main_label->setPosition(offset_x, offset_y + HEIGHT*scale / 2);
}

void GameState::prepareLocalGrid(){
    std::shared_ptr<ClientPlayer> local_player = players[client_id];
    local_player->player.grid = convertGridToColors(game_logic.getGrid());
}

void GameState::drawPlayers(){
    int grid_pixel_width = ((SPACING_PER_RECT+SIDE_LENGTH)*COLUMNS);
    int grid_pixel_height = ((SPACING_PER_RECT+SIDE_LENGTH)*ROWS);
    int free_pixel_x = (WIDTH - grid_pixel_width);
    int free_pixel_y = (HEIGHT);
    

    prepareLocalGrid();
    drawPlayer(client_id, 0, 0, 1.f);

    // Draw Opponent players
    // Calculate the scale
    float scale = 1;
    int a, b, c;

    while(true){
        a = static_cast<int>(std::floor((float)free_pixel_x / (scale * (float)grid_pixel_width)));
        b = static_cast<int>(std::floor((float)free_pixel_y / (scale * (float)grid_pixel_height)));
        c = a * b;

        if(c >= (int)players.size() - 1) break;
        scale /= 2;
    }


    CORE_DEBUG("GameState - drawPlayers - scale: {}, a:{}, b: {}, c:{}", scale, a, b, c);
    int count = 0; // Count of how many grids have been drawn

   
    for(auto[p_client_id, c_player] : players){
        if(p_client_id == client_id) continue;
        if(c_player->player.grid.size() < ROWS-1)continue;

        int row = static_cast<int>((std::floor(count / a)));
        int col = static_cast<int>((std::floor(count % a)));
        CORE_DEBUG("GameState - drawPlayers - Count: {}, Row: {}, Col: {}", count, row, col);

        drawPlayer(
                p_client_id, 
                grid_pixel_width + SPACING_LEFT + static_cast<int>(row * scale * grid_pixel_width), 
                SPACING_TOP + static_cast<int>(col * scale * grid_pixel_height),
                scale);
        count+=1;
    }
}

void GameState::drawUI(){
    data->gui.draw();
}

// Functions to process messages

void GameState::handleGridMessage(yojimbo::Message* t_message){
    GridMessage* message = (GridMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - GridMessage - Player({}", message->client_id);
    if(!players.contains(message->client_id)) return;
    if(message->client_id == client_id) return;
    players[message->client_id]->player.grid = message->grid;
}

void GameState::handleRoundStateChangeMessage(yojimbo::Message* t_message){
    RoundStateChangeMessage* message = (RoundStateChangeMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - RoundStateChangeMessage - RoundState: {}", (int)message->roundstate);
    if(game_id == -1){
        game_id = message->game_id;
    }
    else if(game_id != message->game_id){
        return;
    }
    roundstate = message->roundstate;
};

void GameState::handleTetraminoPlacementMessage(yojimbo::Message* t_message){
    TetraminoPlacementMessage* message = (TetraminoPlacementMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - TetraminoPlacementMessage - TetraminoType: {}", (int)message->tetramino_type);
    if(game_id != message->game_id) return;
    tetramino_queue.push(message->tetramino_type);
};

void GameState::handlePlayerScoreMessage(yojimbo::Message* t_message){
    PlayerScoreMessage* message = (PlayerScoreMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - PlayerScoreMessage - Player({}) Points: {} Position: {}", message->client_id, message->points, message->position);
    if(!players.contains(message->client_id)) return;
    if(game_id != message->game_id) return;
    players[message->client_id]->player.points = message->points;
    players[message->client_id]->player.position = message->position;
};

void GameState::handlePlayerJoinMessage(yojimbo::Message* t_message){
    PlayerJoinMessage* message = (PlayerJoinMessage*) t_message;
    NETWORK_INFO("PROCESS_MESSAGE - PlayerJoinMessage - Player({})", message->client_id);
    if(players.contains(message->client_id)) return;

    std::shared_ptr<ClientPlayer> c_player = std::make_shared<ClientPlayer>();
    c_player->player.client_id = message->client_id;
    
    players.emplace(c_player->player.client_id, c_player);

    initPlayerDrawingGrid(c_player->player.client_id);
    initPlayerUI(c_player->player.client_id);
};

void GameState::handlePlayerLeaveMessage(yojimbo::Message* t_message){
    PlayerLeaveMessage* message = (PlayerLeaveMessage*) t_message;
    NETWORK_INFO("PROCESS_MESSAGE - PlayerLeaveMessage - Player({})", message->client_id);
    if(!players.contains(message->client_id)) return;
    players.erase(message->client_id);
};


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
       data->network_manager.getConnectionStatus() == ConnectionStatus::ERROR_CONNECTION){
        data->state_manager.switchToState(std::make_shared<LoginState>(data));
    }

    if(roundstate == RoundStateType::INGAME && !game_logic.isRunning()){
        game_logic.start();
        game_clock.restart();
    }

    if(game_logic.isFinished()){
        roundstate = RoundStateType::END;
    }

    if(game_id != -1 && roundstate == RoundStateType::INGAME){
        handleKeyboard();
        handleNextTetramino();
        data->network_manager.update();
        game_logic.update(dt);
        frame_counter++;
    }
    updateUI();
    updatePlayerUIs();
};


void GameState::draw(){
    data->window->clear(BACKGROUND_COLOR);
    
    drawPlayers();

    drawUI();
     
    data->window->display();
};
