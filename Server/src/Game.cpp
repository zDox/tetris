#include "Game.hpp"

Game::Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id) : server(t_server), game_id(t_game_id){
    roundstate = RoundStateType::LOBBY;
    min_players = MIN_STARTING_PLAYERS;
    max_players = MAX_PLAYERS;
}

int Game::getGameID(){
    return game_id;
}


GameData Game::getGameData(){
    GameData game_data;
    game_data.game_id = game_id;
    game_data.roundstate = roundstate;
    game_data.min_players = min_players;
    game_data.max_players = max_players;
    for(auto [p_client_id, player] : players){
        game_data.players.push_back(p_client_id);
    }

    return game_data;
}


void Game::addPlayer(uint64_t client_id){
    if(players.contains(client_id)) return;
    std::shared_ptr<GamePlayer> new_player = std::make_shared<GamePlayer>();
    new_player->player.client_id = client_id;
    new_player->gamelogic.init();
    
    players.emplace(client_id, new_player);

    CORE_INFO("Matchmaking - Player({}) joined the game({})", client_id, game_id); if(players.size() >= MIN_STARTING_PLAYERS && !lobby_clock_running){
        lobby_clock_running = true;
        lobby_clock.restart();
        CORE_DEBUG("State - Lobbycountdown in game({}) has started. Remaining {} seconds", game_id, LOBBY_WAIT_TIME);
    }

    broadcastPlayerJoin(client_id);
    broadcastGrid(client_id, new_player->gamelogic.getGrid());

    // Send to him all nescerray data
    for(auto [p_client_id, s_player] : players){
        if(p_client_id == client_id) continue;
        sendPlayerJoin(p_client_id, client_id);
        sendGrid(p_client_id, client_id, players[p_client_id]->gamelogic.getGrid());
    }

    sendGameData(client_id);
}


void Game::removePlayer(uint64_t client_id){
    if(!players.contains(client_id)) return;
    players.erase(client_id);
    broadcastPlayerLeave(client_id);
    CORE_INFO("Matchmaking - Player({}) leaved the game({})", client_id, game_id);
    if(roundstate == RoundStateType::INGAME && players.size() < MIN_INGAME_PLAYERS){
        roundstate = RoundStateType::END;
        broadcastGameData();
    }
    else if(roundstate == RoundStateType::LOBBY && players.size() < MIN_STARTING_PLAYERS){
        lobby_clock_running = false;
        broadcastGameData();
    }
}

bool Game::hasPlayer(uint64_t client_id){
    return players.contains(client_id);
}

std::shared_ptr<GamePlayer> Game::getPlayer(uint64_t client_id){
    if(players.contains(client_id)){
        return players[client_id];
    }
    return nullptr;
}

std::unordered_map<uint64_t, std::shared_ptr<GamePlayer>> Game::getPlayers(){
    return players;
}

bool Game::isFull(){
    return (players.size() >= static_cast<std::vector<GamePlayer>::size_type>(max_players));
}

RoundStateType Game::getRoundState(){
    return roundstate;
}


bool Game::isFinished(){
    int finisher = 0;
    // Count the finisher
    for(auto [p_client_id, s_player] : players){
        if(!s_player->gamelogic.isFinished()) continue;
        finisher +=1;
    }


    if(players.size() < MIN_INGAME_PLAYERS) return true;
    if((int) players.size() - finisher <= 1) return true;

    return false;
}

bool Game::needTimeForPlayoutBuffer(){
    for(auto [client_id, player] : players){
        if(player->gamelogic.isRunning() && player->playout_buffer.size() == 0) return true;
        if(!player->gamelogic.isRunning() && player->playout_buffer.size() < MIN_PLAYOUT_BUFFER) return true;
    }
    return false;
}

bool Game::positionsHaveChanged(){
    bool positions_have_changed = false;
    std::vector<std::shared_ptr<GamePlayer>> sorted;
    // Copy values into vector
    for (auto [p_client_id, player] : players){
        sorted.push_back(player);
    }
    std::sort(sorted.begin(), sorted.end(), compServerPlayer);

    for(std::vector<Player>::size_type i = 0; i < sorted.size(); i++){
        if(players[sorted[i]->player.client_id]->player.position == (long) i+1) continue;
        players[sorted[i]->player.client_id]->player.position = i+1;
        positions_have_changed = true;
    }
    return positions_have_changed;
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

void Game::handleGameFinished(){
    CORE_INFO("Game - Game({}) has finished", game_id);
    // Send Player Scores
    roundstate = RoundStateType::END;
    for(auto [p_client_id, s_player] : players){
        broadcastPlayerData(p_client_id);
    }
    broadcastGameData();
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
    std::shared_ptr<GamePlayer> player = players[client_id];

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

void Game::sendGameData(uint64_t p_client_id){
    int client_index = getPlayersClientIndex(p_client_id);
    GameDataMessage* message = (GameDataMessage*) server->CreateMessage(client_index, (int)MessageType::GAME_DATA);
    message->game_data = getGameData(); 
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
}

void Game::broadcastGameData(){
    for(auto [p_client_id, player] : players){
        sendGameData(p_client_id);
    }
}

void Game::sendGrid(uint64_t sender_id, uint64_t receiver_id, std::vector<std::vector<sf::Color>> grid){
    int client_index = getPlayersClientIndex(receiver_id);
    GridMessage* message = (GridMessage*) server->CreateMessage(client_index, (int)MessageType::GRID);
    message->game_id = game_id;
    message->client_id = sender_id;
    message->grid = convertGridToColors(grid);
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
}

void Game::broadcastGrid(uint64_t client_id, std::vector<std::vector<sf::Color>> grid){
    for(auto [p_client_id, player] : players){
        sendGrid(client_id, p_client_id, grid);
    }
}

void Game::sendPlayerJoin(uint64_t sender_id, uint64_t receiver_id){
    int client_index = getPlayersClientIndex(receiver_id);
    PlayerJoinMessage* message = (PlayerJoinMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_JOIN);
    message->game_id = game_id;
    message->client_id = sender_id;
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
}

void Game::broadcastPlayerJoin(uint64_t client_id){
    // Send all other player his join message
    for(auto [p_client_id, player] : players){
        sendPlayerJoin(client_id, p_client_id);
    } 
}

void Game::broadcastPlayerLeave(uint64_t client_id){
    for(auto [p_client_id, player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        PlayerLeaveMessage* message = (PlayerLeaveMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_LEAVE);
        message->game_id = game_id;
        message->client_id = client_id;
        server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
    }
}

void Game::broadcastPlayerData(uint64_t client_id){
    std::shared_ptr<GamePlayer> s_player = players[client_id];
    Player player = s_player->player;
    for(auto [p_client_id, p_player] : players){
        int client_index = getPlayersClientIndex(p_client_id);
        PlayerDataMessage* message = (PlayerDataMessage*) server->CreateMessage(client_index, (int)MessageType::PLAYER_DATA);
        message->player = player;
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
        broadcastGameData();
        return;
    }
}


void Game::updateIngameState(sf::Time dt){
    if(!gamelogic_running){

        for( auto [client_id, player] : players){
            handleNextTetramino(client_id);
        }
    }

    if(isFinished()){
        handleGameFinished();
        return;
    }

    if(needTimeForPlayoutBuffer()) {
        NETWORK_DEBUG("PlayoutBuffer - Waiting for PlayoutBuffer");
        return; 
    }


    // Start gamelogic if not already started
    if(!gamelogic_running){
        for(auto [client_id, player] : players){
            player->gamelogic.start();
        }
        gamelogic_running = true;
    }



    bool positions_changed = false;
    for(auto [client_id, player] : players){
        // Set next player_input from playout_buffer
        player->gamelogic.setPlayerInput(player->playout_buffer.front());
        player->playout_buffer.pop_front();

        handleNextTetramino(client_id);

        std::vector<std::vector<sf::Color>> old_grid = player->gamelogic.getGrid();
        auto old_points = player->gamelogic.getPoints();
        bool old_finished = player->gamelogic.isFinished();
        player->gamelogic.update(dt);

        bool new_finished = player->gamelogic.isFinished();
        auto new_points = player->gamelogic.getPoints();
        std::vector<std::vector<sf::Color>> new_grid = player->gamelogic.getGrid();

        // handle points have changed
        if(old_points != new_points || old_finished != new_finished){
            player->player.points = new_points;
            if(positionsHaveChanged()){
                positions_changed = true;
            }
            else {
                broadcastPlayerData(client_id);
            }
        }

        if(!vecsAreEqual(old_grid, new_grid)){
            broadcastGrid(client_id, new_grid);
        }
    }

    if(positions_changed){
        NETWORK_TRACE("SEND_MESSAGE - PlayerScoreMessage - Positions have changed ");
        // send to every player every player score
        for(auto [p_client_id, player] : players){
            broadcastPlayerData(p_client_id);
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
