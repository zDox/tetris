#include "GameServer.hpp"

struct ServerAdapter : yojimbo::Adapter{
private:
    std::weak_ptr<GameServer> game_server;
public: 
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator){
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }

    ServerAdapter(std::shared_ptr<GameServer> t_game_server) : game_server(t_game_server){};

    void OnServerClientConnected(int clientIndex){
        if (!game_server.expired()) {
            std::shared_ptr<GameServer> g = game_server.lock();
            g->clientConnected(clientIndex);
        }
    }
    void OnServerClientDisconnected(int clientIndex){
        if (!game_server.expired()) {
            std::shared_ptr<GameServer> g = game_server.lock();
            g->clientDisconnected(clientIndex);
        }
    }
};



// GameServer implementation


// No public constructor, only a factory function,
// so there's no way to have getptr return nullptr.
[[nodiscard]] std::shared_ptr<GameServer> GameServer::create()
{
    // Not using std::make_shared<Best> because the c'tor is private.
    return std::shared_ptr<GameServer>(new GameServer());
}

void GameServer::init(){
    adapter = std::make_unique<ServerAdapter>(this->getPtr());
    connection_config = std::make_unique<yojimbo::ClientServerConfig>();
    *(connection_config) = GameConnectionConfig();
    server = std::make_shared<yojimbo::Server>(
            yojimbo::GetDefaultAllocator(),
            DEFAULT_PRIVATE_KEY,
            yojimbo::Address(SERVER_ADDRESS, SERVER_PORT),
            *connection_config,
            *adapter,
            0.0);
    if(!server) {
        CORE_ERROR("Failed creating server");
        return;
    }
    for(int i=1; i <= 32; i=i+i){
        createGame(i, i*2);
    }
}

std::shared_ptr<GameServer> GameServer::getPtr(){
    return shared_from_this();
}

int GameServer::getPlayersClientIndex(uint64_t client_id){
    int i;
    for(i = 0; i < server->GetNumConnectedClients(); i++){
        if(server->GetClientId(i) == client_id) break;
    }
    return i;
}

std::shared_ptr<Game> GameServer::getPlayersGame(uint64_t client_id){
    for(auto [game_id, game] : games){
        if(game->hasPlayer(client_id)){
            return game;
        }
    }
    return nullptr;
}

void GameServer::loginPlayer(u_int64_t client_id, std::string username){
    if(players.contains(client_id)) return;
    CORE_DEBUG("GameServer - Authentification - Player({}) {} logged in", client_id, username);
    Player player;
    player.client_id = client_id;
    player.name = username;
    std::shared_ptr<ServerPlayer> s_player = std::make_shared<ServerPlayer>();
    s_player->player = player;

    players.emplace(client_id, s_player);
}

void GameServer::logoutPlayer(u_int64_t client_id){
    // Remove player from GameServer
    if(!(players.contains(client_id))) return;
    CORE_DEBUG("GameServer - Authentification - Player({}) {} logged out", client_id, players[client_id]->player.name);
    players.erase(client_id);
    // Remove player from Game
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);
    if(!current_game) return;
    current_game->removePlayer(client_id);
    broadcastGameData(current_game->getGameID());
}

int GameServer::createGame(int min_players, int max_players){ // Returns the game_id of the game it created
    games.emplace(next_game_id, std::make_shared<Game>(server, next_game_id, min_players, max_players));
    next_game_id++;
    CORE_INFO("Created Game with game_id = {}", next_game_id-1);
    broadcastGameData(next_game_id-1);
    return next_game_id-1;
}

void GameServer::processLoginRequest(uint64_t client_id, LoginRequestMessage* message){
    LoginResult result = LoginResult::SUCCESS;
    std::string username = message->username;

    CORE_TRACE("GameServer - Authentification - Received LoginRequestMessage - username: {}", username);
    for(auto [p_client_id, s_player] : players){
        if(s_player->player.name == username){
            result = LoginResult::TAKEN_NAME;
        }
    }

    if(!((2 <= username.size()) && username.size() <= 20)) result = LoginResult::INVALID_NAME;

    if(result == LoginResult::SUCCESS) {
        loginPlayer(client_id, username);
    }

    int client_index = getPlayersClientIndex(client_id);
    LoginResponseMessage* answer_msg = (LoginResponseMessage*) server->CreateMessage(client_index, (int)MessageType::LOGIN_RESPONSE);
    answer_msg->result = result;
    answer_msg->username = username;
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, answer_msg);
}

void GameServer::processGameJoinRequest(uint64_t client_id, GameJoinRequestMessage* message){
    if(!players.contains(client_id)) return;
    int wanted_game_id = message->game_id;
    int client_index = getPlayersClientIndex(client_id);
    GameJoinResponseMessage* answer = (GameJoinResponseMessage*) server->CreateMessage(client_index, (int)MessageType::GAME_JOIN_RESPONSE);
    answer->result = GameJoinResult::SUCCESS;
    answer->game_id = wanted_game_id;

    if(!(games.contains(wanted_game_id))){
        CORE_TRACE("GameServer - MatchMaking - GameJoin from player({}) was unsuccessful. Game ID is invalid", client_id);
        answer->result = GameJoinResult::INVALID_GAME_ID;
    }
    else if(games[wanted_game_id]->isFull()){
        CORE_TRACE("GameServer - MatchMaking - GameJoin from player({}) was unsuccessful. Game is Full", client_id);
        answer->result = GameJoinResult::FULL;
    }
    else if(games[wanted_game_id]->getRoundState() != RoundStateType::LOBBY){
        CORE_TRACE("GameServer - MatchMaking - GameJoin from player({}) was unsuccessful. Game is not in LobbyState", client_id);
        answer->result = GameJoinResult::ALREADY_STARTED;
    }
    server->SendMessage(client_index, (int)GameChannel::RELIABLE, answer);

    if(answer->result != GameJoinResult::SUCCESS) return;
    // GameJoin would be succesfull so add Player to the game
    CORE_TRACE("GameServer - MatchMaking - GameJoin from player({}) was succesfull", client_id);
    games[wanted_game_id]->addPlayer(players[client_id]->player);
    broadcastGameData(wanted_game_id);
}

void GameServer::processGameLeaveRequest(uint64_t client_id, GameLeaveRequestMessage* message){
    if(!games.contains(message->game_id)){
        CORE_TRACE("GameServer - MatchMaking - GameLeave from player({}) - Game ID: {} is invalid", client_id, message->game_id);
        return;
    }
    if(!games[message->game_id]->hasPlayer(client_id)) {
        CORE_TRACE("GameServer - MatchMaking - GameLeave from player({}) - Player is not in game({})", client_id, message->game_id);
        return;
    }
    games[message->game_id]->removePlayer(client_id);
    broadcastGameData(message->game_id);
}

void GameServer::processGameListRequest(uint64_t client_id, GameListRequestMessage* message){
    for(auto [game_id, game] : games){
        sendGameData(client_id, game_id);
    }
}

void GameServer::processMessage(int client_index, yojimbo::Message* message){
    uint64_t client_id = server->GetClientId(client_index);
    switch(message->GetType()){
        case (int)MessageType::PLAYER_INPUT:
        {
            PlayerInputMessage* player_input_message = reinterpret_cast<PlayerInputMessage*>(message);
            if(!games.contains(player_input_message->game_id)) return;
            games[player_input_message->game_id]->processPlayerInput(client_id, player_input_message);
            break;
        }
        case (int)MessageType::LOGIN_REQUEST:
            processLoginRequest(client_id, (LoginRequestMessage*) message);
            break;

        case (int)MessageType::GAME_LIST_REQUEST:
            processGameListRequest(client_id, (GameListRequestMessage*) message);
            break;

        case (int)MessageType::GAME_JOIN_REQUEST:
            processGameJoinRequest(client_id, (GameJoinRequestMessage*) message);
            break;

        case (int)MessageType::GAME_LEAVE_REQUEST:
        {
            processGameLeaveRequest(client_id, (GameLeaveRequestMessage*) message);
            break;
        }
        default:
            break;
    }
}

void GameServer::processMessages(){
    for(int i=0; i< server->GetNumConnectedClients(); i++){
        if(server->IsClientConnected(i)){
            for(int j=0; j < connection_config->numChannels; j++){
                yojimbo::Message* message = server->ReceiveMessage(i, j);
                while (message != NULL){
                    processMessage(i, message);
                    server->ReleaseMessage(i, message);
                    message = server->ReceiveMessage(i, j);
                }
            }
        }
    }
}

void GameServer::updateGames(sf::Time dt){
    for(auto it = games.begin(); it!= games.end();){
        auto& [game_id, game] = *it;

        RoundStateType before_roundstate = game->getRoundState();
        game->update(dt);
        RoundStateType after_roundstate = game->getRoundState();
        if(before_roundstate != after_roundstate){
            broadcastGameData(game_id);
        }

        if(after_roundstate == RoundStateType::DEAD){
            createGame(game->getMinPlayers(), game->getMaxPlayers());
            it = games.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GameServer::sendGameData(uint64_t receiver_id, int game_id){
    if(!games.contains(game_id)) return;
    if(!players.contains(receiver_id)) return;

    int client_index = getPlayersClientIndex(receiver_id);

    CORE_TRACE("GameServer - SendMessage - GameData({}) to Player({})", game_id, players[receiver_id]->player.name);

    GameDataMessage* message = (GameDataMessage*) server->CreateMessage(client_index, (int)MessageType::GAME_DATA);
    message->game_data = games[game_id]->getGameData();
    server->SendMessage(client_index, (int)GameChannel::RELIABLE, message);
}

void GameServer::broadcastGameData(int game_id){
    for(auto [s_client_id, s_player] : players){
        sendGameData(s_client_id, game_id);
    }
}


void GameServer::sendMessages(){ 
}

void GameServer::update(sf::Time dt){
    if(!server->IsRunning()){
        running = false;
        return;
    }
    server->AdvanceTime(game_clock.getElapsedTime().asSeconds());
    server->ReceivePackets();

    processMessages();
    updateGames(dt);

    sendMessages();

    server->SendPackets();
}

void GameServer::clientConnected(int client_index){
}

void GameServer::clientDisconnected(int client_index){
    uint64_t client_id = server->GetClientId(client_index);
    logoutPlayer(client_id);
}

void GameServer::run(){
    sf::Time fixed_dt = sf::seconds(1.0f / (float)TICK_RATE);
    sf::Time last_frame_tick = sf::seconds(0), frame_time, current_time;

    server->Start(MAX_CLIENTS);
    if (!server->IsRunning()){
        CORE_ERROR("GameServer - Couldn't start yojimbo server");
    }
 
    while (running){
        current_time = game_clock.getElapsedTime();
        if(next_cycle <= current_time){
            frame_time = current_time - last_frame_tick;
            last_frame_tick = current_time;
            //CORE_TRACE("PERFORMANCE - Last tick: {}us", frame_time.asMicroseconds());
            update(frame_time);
            next_cycle += fixed_dt;
        }
        else{
            sf::sleep(next_cycle - current_time);
        }
    }

    server->Stop();
    ShutdownYojimbo();
}
