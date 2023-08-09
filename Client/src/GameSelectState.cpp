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
    data->network_manager.queueGameListRequest();
}

void GameSelectState::initUi(){
    main_panel = tgui::ScrollablePanel::create();
    main_panel->setSize(tgui::Layout2d(WIDTH, HEIGHT));

    data->gui.add(main_panel);
}

void GameSelectState::init(){
    initWindow();
    initHandlers();
    initVariables();
    initUi();
}

void GameSelectState::destroy(){
    data->gui.removeAllWidgets();
    data->network_manager.unregisterMessageHandlers();
}


void GameSelectState::joinGame(){
    // TODO: get Game_id from real ui element
    int game_id = 1;

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
        game.join_button = tgui::Button::create();
        game.game_id_label = tgui::Label::create();

        // Setup ui elements
        game.game_panel->setSize(tgui::Layout2d(100, 50));

        game.game_id_label->setText(std::to_string(message->game_data.game_id)); 

        game.join_button->setText("Join");


        game.game_panel->add(game.join_button);
        game.game_panel->add(game.game_id_label);
        main_panel->add(game.game_panel);

        games.emplace(message->game_data.game_id, game);
    }
    else {
        games[message->game_data.game_id].game_data = message->game_data;
    }
}

void GameSelectState::handleGameJoinResponseMessage(yojimbo::Message* t_message){
    GameJoinResponseMessage* message = (GameJoinResponseMessage*) t_message;
    int game_id = message->game_id;
    switch (message->result){
        case GameJoinResult::SUCCESS:
            CORE_DEBUG("GameSelectState - MatchMaking - Joined game({}) succesfully", game_id);
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
        data->gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close(); 
        }
    }
}

void GameSelectState::update(sf::Time dt){
}

void GameSelectState::draw(){
    data->window->clear(BACKGROUND_COLOR);

    data->gui.draw();

    data->window->display();
}
