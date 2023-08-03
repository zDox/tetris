#include "Game.hpp"

Game::Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id) : server(t_server), game_id(t_game_id){
    roundstate = RoundStateType::LOBBY;
}

void Game::addPlayer(uint64_t client_id){
    if(players.contains(client_id)) return;
    std::shared_ptr<ServerPlayer> new_player = std::make_shared<ServerPlayer>();
    new_player->player.client_id = client_id;
    new_player->gamelogic.init();
    
    players.emplace(client_id, new_player);

    CORE_INFO("Matchmaking - Player({}) joined the game({})", client_id, game_id); if(players.size() >= MIN_STARTING_PLAYERS && !lobby_clock_running){
        lobby_clock_running = true;
        lobby_clock.restart();
        CORE_DEBUG("State - Lobbycountdown in game({}) has started. Remaining {} seconds", game_id, LOBBY_WAIT_TIME);
    }

    sendPlayerJoin(client_id);
    sendRoundState(client_id);
}


void Game::removePlayer(uint64_t client_id){
    if(!players.contains(client_id)) return;
    players.erase(client_id);
    sendPlayerLeave(client_id);
    CORE_INFO("Matchmaking - Player({}) leaved the game({})", client_id, game_id);
    if(roundstate == RoundStateType::INGAME && players.size() < MIN_INGAME_PLAYERS){
        roundstate = RoundStateType::END;
        sendRoundStates();
    }
    else if(roundstate == RoundStateType::LOBBY && players.size() < MIN_STARTING_PLAYERS){
        lobby_clock_running = false;
        sendRoundStates();
    }
}

bool Game::hasPlayer(uint64_t client_id){
    return players.contains(client_id);
}

std::shared_ptr<ServerPlayer> Game::getPlayer(uint64_t client_id){
    if(players.contains(client_id)){
        return players[client_id];
    }
    return nullptr;
}

std::unordered_map<uint64_t, std::shared_ptr<ServerPlayer>> Game::getPlayers(){
    return players;
}

RoundStateType Game::getRoundState(){
    return roundstate;
}


bool Game::needTimeForPlayoutBuffer(){
    for(auto [client_id, player] : players){
        if(player->gamelogic.isRunning() && player->playout_buffer.size() == 0) return true;
        if(!player->gamelogic.isRunning() && player->playout_buffer.size() < MIN_PLAYOUT_BUFFER) return true;
    }
    return false;
}

int Game::getPlayersClientIndex(uint64_t client_id){
    int i;
    for(i = 0; i < server->GetNumConnectedClients(); i++){
        if(server->GetClientId(i) == client_id) break;
    }
    return i;
}

// Function sends a new TetraminoType to the client if it
// needs one and is the one with the furthest progression in the tetramino_sequenz
void Game::handleNextTetramino(uint64_t client_id){
    if(!players.contains(client_id)) return;
    if(!players[client_id]->gamelogic.isNeedingNextTetramino()) return;
    players[client_id]->tetramino_cursor++;

    if(players[client_id]->tetramino_cursor >= (int) tetramino_sequenz.size()-1){
        // Need new tetramino to be generated
        tetramino_sequenz.push_back(static_cast<TetraminoType>(std::rand() % ((int)TetraminoType::AMOUNT - 1)));;
    }
    TetraminoType next_tetramino_type = tetramino_sequenz[players[client_id]->tetramino_cursor];

    // Setting it in own gamelogic
    players[client_id]->gamelogic.setNextTetramino(next_tetramino_type);
    // Sending the TetraminoPlacement Message to the Client
    int client_index = getPlayersClientIndex(client_id);
    TetraminoPlacementMessage* message = (TetraminoPlacementMessage*) server->CreateMessage(client_index, (int)MessageType::TETRAMINO_PLACEMENT);
    message->game_id = game_id;
    message->tetramino_type = next_tetramino_type;
    server->SendMessage(client_index, (int)GameChannel::RELIABLE, message);
}

void Game::processPlayerInputMessage(uint64_t client_id, PlayerInputMessage* message){
    if(!players.contains(client_id)) return;
    if(game_id != message->game_id) return;
    /*
    NETWORK_TRACE("PROCESS_MESSAGE - PlayerInputMessage - game_id: {}, client_id: {}, \nleft: {}, right: {}, up: {}, down: {}", 
            message->game_id,
            client_id,
            message->player_input.left, 
            message->player_input.right, 
            message->player_input.up, 
            message->player_input.down);
    */
    std::shared_ptr<ServerPlayer> player = players[client_id];

    if(player->playout_buffer.size() == 0){
        player->playout_buffer.push_back(message->player_input);
        return;
    }

    if(player->playout_buffer.back().frame +1 != message->player_input.frame){
        NETWORK_WARN("PROCESS_MESSAGE - PlayerInputMessage - PlayerInput came in wrong order last frame: {}, new frame: {}", 
                player->playout_buffer.back().frame, 
                message->player_input.frame);
        auto it = std::lower_bound(player->playout_buffer.begin(), player->playout_buffer.end(), message->player_input.frame, comp);
        player->playout_buffer.insert(it, message->player_input);
    }
    else {
        player->playout_buffer.push_back(message->player_input);
    }
}

void Game::sendRoundState(uint64_t client_id){
    int client_index = getPlayersClientIndex(client_id);
    RoundStateChangeMessage* message = (RoundStateChangeMessage*) server->CreateMessage(client_index, (int)MessageType::ROUNDSTATECHANGE);
    message->game_id = game_id;
    message->roundstate = roundstate;
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
}

void Game::sendRoundStates(){
    for(auto [p_client_id, player] : players){
        sendRoundState(p_client_id);
    }
}

void Game::sendGrid(uint64_t client_id, std::vector<std::vector<sf::Color>> grid){
    for(auto [p_client_id, player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        GridMessage* message = (GridMessage*) server->CreateMessage(client_index, (int)MessageType::GRID);
        message->game_id = game_id;
        message->client_id = client_id;
        message->grid = convertGridToColors(grid);
        server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
    }
}

void Game::sendPlayerJoin(uint64_t client_id){
    // Send all other player his join message
    for(auto [p_client_id, player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        PlayerJoinMessage* message = (PlayerJoinMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_JOIN);
        message->game_id = game_id;
        message->client_id = client_id;
        server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
    }
    // Send him all players that are already connected
    int client_index = getPlayersClientIndex(client_id);
    for(auto [p_client_id, player] : players){
        if(p_client_id == client_id) continue;
        PlayerJoinMessage* message = (PlayerJoinMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_JOIN);
        message->game_id = game_id;
        message->client_id = p_client_id;
        server->SendMessage(client_index, (int)GameChannel::RELIABLE, message);
    }
}

void Game::sendPlayerLeave(uint64_t client_id){
    for(auto [p_client_id, player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        PlayerLeaveMessage* message = (PlayerLeaveMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_LEAVE);
        message->game_id = game_id;
        message->client_id = client_id;
        server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
    }
}

void Game::sendPlayerScore(uint64_t client_id){
    std::shared_ptr<ServerPlayer> player = players[client_id];
    for(auto [p_client_id, p_player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        PlayerScoreMessage* message = (PlayerScoreMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_SCORE);
        message->game_id = game_id;
        message->client_id = client_id;
        message->points = player->player.points;
        message->position = player->player.position;
        server->SendMessage(client_index, (int)GameChannel::RELIABLE, message);
    }
}


void Game::updateLobbyState(sf::Time dt){
    if(lobby_clock.getElapsedTime() >= sf::seconds(LOBBY_WAIT_TIME) && lobby_clock_running){
        if(players.size() < MIN_STARTING_PLAYERS) {
            return;
        }
        roundstate = RoundStateType::INGAME;
        CORE_TRACE("Game - Game ({}): Switched to Ingame State", game_id);
        sendRoundStates();
        return;
    }
}


void Game::updateIngameState(sf::Time dt){
    if(!gamelogic_running){

        for( auto [client_id, player] : players){
            handleNextTetramino(client_id);
        }
    }

    if(needTimeForPlayoutBuffer()) {
        NETWORK_DEBUG("PlayoutBuffer - Waiting for PlayoutBuffer");
        return; 
    }

    if(!gamelogic_running){
        for(auto [client_id, player] : players){
            player->gamelogic.start();
        }
        gamelogic_running = true;
    }

    for(auto [client_id, player] : players){
        // Set next player_input from playout_buffer
        player->gamelogic.setPlayerInput(player->playout_buffer.front());
        player->playout_buffer.pop_front();

        handleNextTetramino(client_id);

        std::vector<std::vector<sf::Color>> old_grid = player->gamelogic.getGrid();
        auto old_points = player->gamelogic.getPoints();
        player->gamelogic.update(dt);

        auto new_points = player->gamelogic.getPoints();
        std::vector<std::vector<sf::Color>> new_grid = player->gamelogic.getGrid();

        if(old_points != new_points){
            player->player.points = new_points;
            sendPlayerScore(client_id);
        }

        if(!vecsAreEqual(old_grid, new_grid)){
            sendGrid(client_id, new_grid);
        }
    }
}

void Game::updateEndState(sf::Time dt){
}

void Game::update(sf::Time dt){
    switch(roundstate){
        case RoundStateType::LOBBY:
           updateLobbyState(dt);
           break;
        case RoundStateType::INGAME:
           updateIngameState(dt);
           break;
        case RoundStateType::END:
           updateEndState(dt);
           break;
        default:
           return;
    }
}
