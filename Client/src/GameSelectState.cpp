#include "GameState.hpp"
#include "GameSelectState.hpp"

GameSelectState::GameSelectState(std::shared_ptr<ApplicationData> t_data):data(t_data){};

void GameSelectState::initWindow(){
    data->window->setFramerateLimit(MENU_FPS);
}

void GameSelectState::initHandlers(){
    data->network_manager.registerMessageHandler(
            MessageType::GAME_JOIN_RESPONSE, 
            std::bind(&GameSelectState::handleGameJoinResponseMessage, this, std::placeholders::_1));
    data->network_manager.registerMessageHandler(
            MessageType::GAME_DATA, 
            std::bind(&GameSelectState::handleGameDataMessage, this, std::placeholders::_1));


}
void GameSelectState::initVariables(){
    data->network_manager.start();
    data->network_manager.queueGameListRequest();
}

void GameSelectState::initUi(){
    main_panel = tgui::ScrollablePanel::create();
    main_panel->setSize({"100%", "100%"});

    data->gui->add(main_panel);
}

void GameSelectState::init(){
    initWindow();
    initHandlers();
    initVariables();
    initUi();
}

void GameSelectState::destroy(){
    data->gui->remove(main_panel);
    data->network_manager.unregisterMessageHandlers();
}


void GameSelectState::joinGame(int game_id){
    if(!(games.contains(game_id))) return;

    data->network_manager.queueGameJoinRequest(game_id);
}

void GameSelectState::handleGameDataMessage(yojimbo::Message* t_message){
    GameDataMessage* message = (GameDataMessage*) t_message;
    CORE_TRACE("GameSelectState - GameList - Received GameData for game({})", message->game_data.game_id);
    if(!(games.contains(message->game_data.game_id))){
        // Create new game and init it's ui elements
        SelectableGame game;
        game.game_data = message->game_data;
        game.game_panel = tgui::Panel::create();
        game.heading = tgui::Label::create();
        game.players_label = tgui::Label::create();
        game.players_text = tgui::Label::create();
        game.status_label = tgui::Label::create();
        game.status_text = tgui::Label::create();
        game.join_button = tgui::Button::create();

        // Setup ui elements
        game.game_panel->setSize(tgui::Layout2d(GAME_PANEL_WIDTH, GAME_PANEL_HEIGHT));
        game.game_panel->getRenderer()->setBackgroundColor(tgui::Color::Blue);
        game.game_panel->getRenderer()->setBorderColor(tgui::Color::Black);

        game.heading->setText("Game - " + std::to_string(message->game_data.game_id)); 
        game.heading->setOrigin(0.5f, 0);
        game.heading->setPosition("50%", 5);

        // Display of how connected players and max players
        game.players_label->setText("Players:");
        game.players_label->setPosition(7, 10);

        game.players_text->setText("0/0");
        game.players_text->setOrigin(1.f,0);
        game.players_text->setPosition(GAME_PANEL_WIDTH-5, tgui::bindTop(game.players_label));

        // Display of game status e.g. Lobby
        game.status_label->setText("Status:");
        game.status_label->setPosition(tgui::bindLeft(game.players_label), tgui::bindBottom(game.players_label)+5);

        game.status_text->setText("Unknown");
        game.status_text->setOrigin(1.f,0);
        game.status_text->setPosition(GAME_PANEL_WIDTH-5, tgui::bindTop(game.status_label));

        game.join_button->setText("Join");
        game.join_button->setUserData(game.game_data.game_id);
        game.join_button->onPress(&GameSelectState::joinGame, this, game.game_data.game_id);
        game.join_button->setOrigin(0.5f, 1.f);
        game.join_button->setPosition("50%", GAME_PANEL_HEIGHT - 5);

        game.game_panel->add(game.heading);
        game.game_panel->add(game.players_label);
        game.game_panel->add(game.players_text);
        game.game_panel->add(game.status_label);
        game.game_panel->add(game.status_text);
        game.game_panel->add(game.join_button);
        main_panel->add(game.game_panel);

        games.emplace(message->game_data.game_id, game);
    }
    else {
        games[message->game_data.game_id].game_data = message->game_data;
        if(message->game_data.roundstate == RoundStateType::DEAD){
            games[message->game_data.game_id].game_panel->removeAllWidgets();
            main_panel->remove(games[message->game_data.game_id].game_panel);

            games.erase(message->game_data.game_id);
        }
    }
}

void GameSelectState::handleGameJoinResponseMessage(yojimbo::Message* t_message){
    GameJoinResponseMessage* message = (GameJoinResponseMessage*) t_message;
    int game_id = message->game_id;
    switch (message->result){
        case GameJoinResult::SUCCESS:
            CORE_DEBUG("GameSelectState - MatchMaking - Joined game({}) succesfully", game_id);
            data->game_id = game_id;
            data->network_manager.setGameID(game_id);
            data->network_manager.stop();
            data->state_manager.switchToState(std::make_shared<GameState>(data));
            break;
        case GameJoinResult::FULL:
            CORE_DEBUG("GameSelectState - MatchMaking - Game({}) is already full", game_id);
            break;
        case GameJoinResult::ALREADY_STARTED:
            CORE_DEBUG("GameSelectState - MatchMaking - Game({}) is already full", game_id);
            break;
        case GameJoinResult::INVALID_GAME_ID:
            CORE_DEBUG("GameSelectState - MatchMaking - Game({}) is already full", game_id);
            break;
        default:
            CORE_WARN("GameSelectState - Unhandled GameJoinResult");
    }
}

void GameSelectState::handleInputs(){
    sf::Event event;
    while(data->window->pollEvent(event)){
        data->gui->handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close(); 
        }
    }
}

void GameSelectState::updateGameUIs(){
    int count = 0;
    int elements_per_row = static_cast<int>(std::floor((data->config->getInt("WIDTH") - GAME_PANEL_SPACING_ROW) / (GAME_PANEL_WIDTH + GAME_PANEL_SPACING_ROW)));
    for(auto [game_id, game] : games){
        game.players_text->setText(std::to_string(game.game_data.players.size()) + "/" + std::to_string(game.game_data.max_players));
        game.status_text->setText(std::to_string(game.game_data.roundstate));
        game.game_panel->setPosition(
                (count % elements_per_row) * (GAME_PANEL_WIDTH + GAME_PANEL_SPACING_ROW) + GAME_PANEL_SPACING_ROW,
                (count / elements_per_row) * (GAME_PANEL_HEIGHT + GAME_PANEL_SPACING_COLUMN) + GAME_PANEL_SPACING_COLUMN 
        );
        count++;
    }

}

void GameSelectState::updateUI(){
    updateGameUIs();
}

void GameSelectState::update(sf::Time dt){
    updateUI();
}

void GameSelectState::draw(){
    data->window->clear(BACKGROUND_COLOR);

    data->gui->draw();

    data->window->display();
}
