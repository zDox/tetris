#include "NetworkManager.hpp"
#include "network.hpp"

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
        std::cerr << "ERROR: Failed to Initialize yojimbo\n";
        return false;
    }
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_DEBUG );
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
    if(!players.contains(message->client_id)) return;
    players[message->client_id].grid = message->grid;
}

void NetworkManager::processRoundStateChangeMessage(RoundStateChangeMessage* message){
    if(game_id == -1){
        game_id = message->game_id;
    }
    else if(game_id != message->game_id){
        return;
    }
    roundstate = message->roundstate;
}

void NetworkManager::processTetraminoPlacementMessage(TetraminoPlacementMessage* message){
    if(game_id != message->game_id) return;
    tetramino_queue.push(message->tetramino_type);
}

void NetworkManager::processPlayerScoreMessage(PlayerScoreMessage* message){
    if(!players.contains(message->client_id)) return;
    if(game_id != message->game_id) return;
    players[message->client_id].points = message->points;
    players[message->client_id].position = message->position;
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

                case (int) MessageType::PLAYERSCORE:
                    processPlayerScoreMessage((PlayerScoreMessage*) message);
                    break;
            }
            client->ReleaseMessage(message);
            message = client->ReceiveMessage(i);
        }
    }
}


void NetworkManager::queuePlayerCommand(PlayerCommandType command_type){
    player_command_queue.push(command_type);
}

void NetworkManager::sendPlayerCommands(){
    while(!player_command_queue.empty()){
        PlayerCommandMessage* message = (PlayerCommandMessage*) client->CreateMessage((int)MessageType::PLAYER_COMMAND);
        message->game_id = game_id;
        message->command_type = player_command_queue.front();
        player_command_queue.pop();
        client->SendMessage((int)GameChannel::RELIABLE, message);
    }
}

void NetworkManager::sendMessages(){
    sendPlayerCommands();
}

std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> NetworkManager::getOpponentsGrid(){
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> opponents_grid;
    for(auto [client_id, player] : players){
        opponents_grid[client_id] = player.grid;
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
