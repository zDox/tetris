#include "GameSelectState.hpp"
#include "GameState.hpp"

GameState::GameState(std::shared_ptr<ApplicationData> t_data) : data(t_data) {
    client_id = data->network_manager.getClientID();
    game_id = data->game_id;
}

void GameState::init(){
    initWindow();
    initVariables();
    initHandlers();
    game_logic.init();
    initUI();
}

void GameState::destroy(){
    data->app_overlay->removeButton("leave_lobby");
    data->gui->remove(main_panel);
    data->network_manager.setGameID(-1);
    data->network_manager.stop();
    data->network_manager.unregisterMessageHandlers();
};

void GameState::leaveLobby(){
    data->network_manager.queueGameLeaveRequest();
    data->state_manager.switchToState(std::make_shared<GameSelectState>(data));
}

void GameState::initWindow(){  
    data->window->setFramerateLimit(data->config->getInt("FRAME_LIMIT"));
}

void GameState::initVariables(){
    data->network_manager.start();

    main_panel = tgui::Panel::create({"100%", "100%"});
    main_panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    data->gui->add(main_panel);

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
            MessageType::GAME_DATA, 
            std::bind(&GameState::handleGameDataMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::TETRAMINO_PLACEMENT,
            std::bind(&GameState::handleTetraminoPlacementMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_DATA, 
            std::bind(&GameState::handlePlayerDataMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_JOIN, 
            std::bind(&GameState::handlePlayerJoinMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::PLAYER_LEAVE, 
            std::bind(&GameState::handlePlayerLeaveMessage, this, std::placeholders::_1));

    data->app_overlay->addButton(8, "leave_lobby", "Leave Lobby", std::bind(&GameState::leaveLobby, this));
}

void GameState::initUI(){
    game_exit_button = tgui::Button::create("Exit Lobby");
    game_exit_button->setVisible(false);
    game_exit_button->setOrigin(1,1);
    game_exit_button->setPosition("95%", "95%");
    game_exit_button->onPress(&GameState::leaveLobby, this);
    main_panel->add(game_exit_button);
}


void GameState::initPlayerDrawingGrid(uint64_t p_client_id){
    if(!players.contains(p_client_id)) return;
    float side_length = (static_cast<float>((data->config->getInt("HEIGHT") - SPACING_TOP - SPACING_BOTTOM) - static_cast<float>(ROWS) * SPACING_PER_RECT)) / static_cast<float>(ROWS);
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];
    c_player->drawing_grid.resize(ROWS);
    for(int i=0; i<ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            std::shared_ptr<sf::RectangleShape> rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(side_length, side_length));
            rect->setFillColor(GRID_COLOR);
            rect->setPosition(k*side_length + k*SPACING_PER_RECT + SPACING_LEFT, i*side_length + i*SPACING_PER_RECT + SPACING_TOP);
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

    main_panel->add(c_player->main_label);
    main_panel->add(c_player->stats_label);
}

void GameState::updateUI(){
}

void GameState::updatePlayerUI(uint64_t p_client_id){
    if(!players.contains(p_client_id)) return;
    std::shared_ptr<ClientPlayer> c_player = players[p_client_id];

    tgui::String stats_text = 
        "Username: " + c_player->player.name+
        "\npoints: " + std::to_string(c_player->player.points) + 
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
            main_text = "Place " + std::to_string(c_player->player.position) + ".\n" + c_player->player.name;
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

void GameState::handleKeyboard(sf::Time dt){
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
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && forced_falling_time <= sf::seconds(0)){
            player_input.down = true;
            forced_falling_time = sf::Time(sf::seconds(1/data->config->getDouble("FORCED_FALLING_SPEED")));
        }
        else {
            forced_falling_time -= dt;
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
    // CORE_TRACE("GameState - drawPlayer - off_x: {}, off_y: {}", offset_x, offset_y);
    float side_length = (static_cast<float>((data->config->getInt("HEIGHT") - SPACING_TOP - SPACING_BOTTOM) - static_cast<float>(ROWS) * SPACING_PER_RECT)) / static_cast<float>(ROWS);
    // Draw the grid 
    for(int i = 0; i < ROWS; i++){
        for(int k = 0; k < COLUMNS; k++){
            int pos_x = k*side_length*scale + k*SPACING_PER_RECT*scale + offset_x;
            int pos_y = i*side_length*scale + i*SPACING_PER_RECT*scale + offset_y;
            uint32_t color_uint = c_player->player.grid[i][k];
            uint8_t red = (color_uint >> 24) & 0xFF;
            uint8_t green = (color_uint >> 16) & 0xFF;
            uint8_t blue = (color_uint >> 8) & 0xFF;
            uint8_t alpha = color_uint & 0xFF;
            sf::Color color(red, green, blue, alpha);
            std::shared_ptr<sf::RectangleShape> rect = players[p_client_id]->drawing_grid[i][k];
            rect->setSize(sf::Vector2(side_length*scale, side_length*scale));
            rect->setPosition(pos_x, pos_y);
            rect->setFillColor(color);
            data->window->draw(*rect);
        }
    }

    // Draw UI Elements
    c_player->stats_label->setPosition(offset_x, offset_y);
    c_player->main_label->setPosition(offset_x, offset_y + data->config->getInt("HEIGHT") *scale / 2);
}

void GameState::prepareLocalGrid(){
    std::shared_ptr<ClientPlayer> local_player = players[client_id];
    local_player->player.grid = convertGridToColors(game_logic.getGrid());
}

void GameState::drawPlayers(){
    float side_length = (static_cast<float>((data->config->getInt("HEIGHT") - SPACING_TOP - SPACING_BOTTOM) - static_cast<float>(ROWS) * SPACING_PER_RECT)) / static_cast<float>(ROWS);
    int grid_pixel_width = ((SPACING_PER_RECT+side_length)*COLUMNS);
    int grid_pixel_height = ((SPACING_PER_RECT+side_length)*ROWS);
    int free_pixel_x = (data->config->getInt("WIDTH") - SPACING_LEFT - grid_pixel_width);
    int free_pixel_y = (data->config->getInt("HEIGHT") - SPACING_TOP);
    

    prepareLocalGrid();
    drawPlayer(client_id, SPACING_LEFT, SPACING_TOP, 1.f);

    // Draw Opponent players
    // Calculate the scale
    float scale = 1;
    int a, b, c;

    while(true){
        a = static_cast<int>(std::floor((float)free_pixel_x / (scale * (float)(grid_pixel_width + SPACING_BETWEEN_GRIDS))));
        b = static_cast<int>(std::floor((float)free_pixel_y / (scale * (float)(grid_pixel_height + SPACING_BETWEEN_GRIDS))));
        c = a * b;

        if(c >= (int)players.size() - 1) break;
        scale = scale - 0.05;
    }


    // CORE_TRACE("GameState - drawPlayers - scale: {}, a:{}, b: {}, c:{}", scale, a, b, c);
    int count = 0; // Count of how many grids have been drawn

   
    for(auto[p_client_id, c_player] : players){
        if(p_client_id == client_id) continue;
        if(c_player->player.grid.size() < ROWS-1){
            CORE_WARN("GameState - drawPlayers - player({}) has to small grid size({}).", p_client_id, c_player->player.grid.size());
            continue;
        };

        int row = static_cast<int>((std::floor(count % a)));
        int col = static_cast<int>((std::floor(count / a)));
        // CORE_TRACE("GameState - drawPlayers - Count: {}, Row: {}, Col: {}", count, row, col);

        drawPlayer(
                p_client_id, 
                grid_pixel_width + SPACING_LEFT + static_cast<int>((row+1) * SPACING_BETWEEN_GRIDS * scale + row * scale * grid_pixel_width), 
                SPACING_TOP + static_cast<int>(col * SPACING_BETWEEN_GRIDS * scale + col * scale * grid_pixel_height),
                scale);
        count+=1;
    }
}

void GameState::drawUI(){
    data->gui->draw();
}

void GameState::roundStateChange(){
    switch (roundstate){
        case RoundStateType::LOBBY:
            break;
        case RoundStateType::INGAME:
            break;
        case RoundStateType::END:
            game_exit_button->setVisible(true);
            break;
        case RoundStateType::DEAD:
            leaveLobby();
            break;
        case RoundStateType::COUNT:
            break;
    }
};

// Functions to process messages

void GameState::handleGridMessage(yojimbo::Message* t_message){
    GridMessage* message = (GridMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - GridMessage - Player({})", message->client_id);
    if(!players.contains(message->client_id)) return;
    if(message->client_id == client_id) return;
    players[message->client_id]->player.grid = message->grid;
}

void GameState::handleGameDataMessage(yojimbo::Message* t_message){
    GameDataMessage* message = (GameDataMessage*) t_message;
    GameData game_data = message->game_data;
    NETWORK_TRACE("PROCESS_MESSAGE - GameDataMessage - RoundState: {}", (int)game_data.roundstate);
    if(game_id != game_data.game_id){
        return;
    }
    RoundStateType previous = roundstate; 
    roundstate = game_data.roundstate;
    if(previous != roundstate){
        roundStateChange();
    }
}

void GameState::handleTetraminoPlacementMessage(yojimbo::Message* t_message){
    TetraminoPlacementMessage* message = (TetraminoPlacementMessage*) t_message;
    NETWORK_TRACE("PROCESS_MESSAGE - TetraminoPlacementMessage - TetraminoType: {}", (int)message->tetramino_type);
    if(game_id != message->game_id) return;
    tetramino_queue.push(message->tetramino_type);
};

void GameState::handlePlayerDataMessage(yojimbo::Message* t_message){
    PlayerDataMessage* message = (PlayerDataMessage*) t_message;
    Player p = message->player;
    NETWORK_TRACE("PROCESS_MESSAGE - PlayerDataMessage - Player({}) Points: {} Position: {}", p.client_id, p.points, p.position);
    if(!players.contains(p.client_id)) return;
    if(game_id != message->game_id) return;
    players[p.client_id]->player = p;
};

void GameState::handlePlayerJoinMessage(yojimbo::Message* t_message){
    PlayerJoinMessage* message = (PlayerJoinMessage*) t_message;
    NETWORK_INFO("PROCESS_MESSAGE - PlayerJoinMessage - Player({})", message->client_id);
    if(players.contains(message->client_id)) return;

    std::shared_ptr<ClientPlayer> c_player = std::make_shared<ClientPlayer>();
    c_player->player.client_id = message->client_id;
    
    players.emplace(c_player->player.client_id, c_player);

    initPlayerDrawingGrid(message->client_id);
    initPlayerUI(message->client_id);
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
        data->gui->handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close();
        }
        if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Escape){
                data->app_overlay->toggle();
            }
        }
        if(event.type == sf::Event::Closed){
            data->app_overlay->setEnabled(true);
        }
    }
};


void GameState::update(sf::Time dt){ 
    data->network_manager.update();
    if(data->network_manager.getConnectionStatus() == ConnectionStatus::DISCONNECTED or
       data->network_manager.getConnectionStatus() == ConnectionStatus::ERROR_CONNECTION){
        data->state_manager.switchToState(std::make_shared<LoginState>(data));
    }

    if(roundstate == RoundStateType::INGAME && !game_logic.isFinished() && !game_logic.isRunning()){
        game_logic.start();
        game_clock.restart();
    }

    if(roundstate == RoundStateType::INGAME){
        handleKeyboard(dt);
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
}
