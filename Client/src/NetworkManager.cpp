#include "NetworkManager.hpp"

NetworkManager::NetworkManager(){
    generateClientID();
}

ConnectionStatus NetworkManager::getConnectionStatus(){
    yojimbo::ClientState state = client->GetClientState();
    switch(state){
        case yojimbo::ClientState::CLIENT_STATE_CONNECTED:
            return ConnectionStatus::CONNECTED;
        case yojimbo::ClientState::CLIENT_STATE_CONNECTING:
            return ConnectionStatus::CONNECTING;
        case yojimbo::ClientState::CLIENT_STATE_DISCONNECTED:
            return ConnectionStatus::DISCONNECTED;
        case yojimbo::ClientState::CLIENT_STATE_ERROR:
            return ConnectionStatus::ERROR;
    }
    return ConnectionStatus::ERROR;
}

int NetworkManager::getGameID(){
    return game_id;
}

RoundStateType NetworkManager::getRoundState(){
    return roundstate;
}

TetraminoType NetworkManager::getNextTetramino(){
    if(tetramino_queue.empty()) throw std::out_of_range("Tetramino queue is empty not able to retrive element!");
    TetraminoType next = tetramino_queue.front();
    tetramino_queue.pop();
    return next;
}

void NetworkManager::generateClientID(){
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());

    // Generate a random value for clientId
    client_id = distribution(generator);
}

bool NetworkManager::init(){
    if (!InitializeYojimbo() ){
        // NETWORK_ERROR("ERROR: Failed to Initialize yojimbo\n");
        return false;
    }
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );
    std::srand((unsigned int) time(NULL));
    connection_config = std::make_shared<yojimbo::ClientServerConfig>();
    *(connection_config) = GameConnectionConfig();
    client = std::make_shared<yojimbo::Client>(
            yojimbo::GetDefaultAllocator(), 
            yojimbo::Address("0.0.0.0"), 
            *connection_config, 
            adapter, 
            0.0);
    return true;
}

void NetworkManager::destroy(){
    ShutdownYojimbo();
}


void NetworkManager::connect(std::string text_address){
    if(client->IsConnecting()) return;

    yojimbo::Address server_address(text_address.c_str());
    if(!server_address.IsValid()) return;
    if(server_address.GetPort() == 0) server_address.SetPort(SERVER_PORT);

    client->InsecureConnect(DEFAULT_PRIVATE_KEY, client_id, server_address);
} 

void NetworkManager::disconnect(){
    client->Disconnect();
}

void NetworkManager::processGridMessage(GridMessage* message){
    NETWORK_TRACE("PROCESS_MESSAGE - GridMessage - Player({}", message->client_id);
    if(!players.contains(message->client_id)) return;
    players[message->client_id].grid = message->grid;
}

void NetworkManager::processRoundStateChangeMessage(RoundStateChangeMessage* message){
    NETWORK_TRACE("PROCESS_MESSAGE - RoundStateChangeMessage - RoundState: {}", (int)message->roundstate);
    if(game_id == -1){
        game_id = message->game_id;
    }
    else if(game_id != message->game_id){
        return;
    }
    roundstate = message->roundstate;
}

void NetworkManager::processTetraminoPlacementMessage(TetraminoPlacementMessage* message){
    NETWORK_TRACE("PROCESS_MESSAGE - TetraminoPlacementMessage - TetraminoTye: {}", (int)message->tetramino_type);
    if(game_id != message->game_id) return;
    tetramino_queue.push(message->tetramino_type);
}

void NetworkManager::processPlayerScoreMessage(PlayerScoreMessage* message){
    NETWORK_TRACE("PROCESS_MESSAGE - PlayerScoreMessage - Player({}) Points: {} Position: {}", message->client_id, message->points, message->position);
    if(!players.contains(message->client_id)) return;
    if(game_id != message->game_id) return;
    players[message->client_id].points = message->points;
    players[message->client_id].position = message->position;
}

void NetworkManager::processPlayerJoinMessage(PlayerJoinMessage* message){
    NETWORK_INFO("PROCESS_MESSAGE - PlayerJoinMessage - Player({})", message->client_id);
    if(players.contains(message->client_id)) return;
    Player player;
    player.client_id = message->client_id;
    players.emplace(message->client_id, player);

}

void NetworkManager::processPlayerLeaveMessage(PlayerLeaveMessage *message) {
    NETWORK_INFO("PROCESS_MESSAGE - PlayerLeaveMessage - Player({})", message->client_id);
    if(!players.contains(message->client_id)) return;
    players.erase(message->client_id);
}

void NetworkManager::processMessages(){
    for (int i = 0; i < connection_config->numChannels; i++) {
        yojimbo::Message* message = client->ReceiveMessage(i);
        while (message != NULL) {
            switch (message->GetType()){
                case (int) MessageType::GRID:
                    processGridMessage((GridMessage*)message);
                    break;
                    
                case (int) MessageType::ROUNDSTATECHANGE:
                    processRoundStateChangeMessage((RoundStateChangeMessage*) message);
                    break;

                case (int) MessageType::TETRAMINO_PLACEMENT:
                    processTetraminoPlacementMessage((TetraminoPlacementMessage*) message);
                    break;

                case (int) MessageType::PLAYER_JOIN:
                    processPlayerJoinMessage((PlayerJoinMessage*) message);
                    break;

                case (int) MessageType::PLAYER_LEAVE:
                    processPlayerLeaveMessage((PlayerLeaveMessage*) message);
                    break;

                case (int) MessageType::PLAYER_SCORE:
                    processPlayerScoreMessage((PlayerScoreMessage*) message);
                    break;
            }
            client->ReleaseMessage(message);
            message = client->ReceiveMessage(i);
        }
    }
}


void NetworkManager::queuePlayerInput(PlayerInput t_player_input){
    player_input = std::make_shared<PlayerInput>();
    *player_input = t_player_input;
}

void NetworkManager::sendPlayerInput(){
    if(!player_input) return;

    PlayerInputMessage* message = (PlayerInputMessage*) client->CreateMessage((int)MessageType::PLAYER_INPUT);
    message->game_id = game_id;
    message->player_input = *player_input;
    client->SendMessage((int)GameChannel::RELIABLE, message);
    player_input = nullptr;
    NETWORK_TRACE("SEND_MESSAGE - PlayerInputMessage - left: {}, right: {}, up: {}, down: {}", 
            message->player_input.left, 
            message->player_input.right, 
            message->player_input.up, 
            message->player_input.down);
}

void NetworkManager::sendMessages(){
    sendPlayerInput();
}

std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> NetworkManager::getOpponentsGrid(){
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> opponents_grid;
    for(auto [p_client_id, player] : players){
        if(player.grid.size() == 0) continue;
        opponents_grid[p_client_id] = player.grid;
    }
    return opponents_grid;
}

void NetworkManager::update(){
    if(next_cycle > network_clock.getElapsedTime()) return;
    client->AdvanceTime(network_clock.getElapsedTime().asSeconds());
    client->ReceivePackets();

    if(client->IsConnected()){
        processMessages();

        // Sending Messages
        sendMessages();
    }
    client->SendPackets();
    next_cycle += sf::seconds(1.f/TICK_RATE);
};
