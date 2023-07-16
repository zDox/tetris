#include "NetworkManager.hpp"

NetworkManager::NetworkManager(){
    generateClientID();
}

CONNECTION_STATUS NetworkManager::getConnectionStatus(){
    yojimbo::ClientState state = client->GetClientState();
    switch(state){
        case yojimbo::ClientState::CLIENT_STATE_CONNECTED:
            return CONNECTION_STATUS::CONNECTED;
        case yojimbo::ClientState::CLIENT_STATE_CONNECTING:
            return CONNECTION_STATUS::CONNECTING;
        case yojimbo::ClientState::CLIENT_STATE_DISCONNECTED:
            return CONNECTION_STATUS::DISCONNECTED;
        case yojimbo::ClientState::CLIENT_STATE_ERROR:
            return CONNECTION_STATUS::ERROR;
    }
    return CONNECTION_STATUS::ERROR;
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
    ClientAdapter adapter;
    client = std::make_shared<yojimbo::Client>(
            yojimbo::GetDefaultAllocator(), 
            yojimbo::Address("0.0.0.0"), 
            game_connection_config, 
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
    if (message->client_id == client_id) return;
    opponents_grid[message->client_id] = message->grid;
}

void NetworkManager::processMessages(){
    for (int i = 0; i < connection_config->numChannels; i++) {
        yojimbo::Message* message = client->ReceiveMessage(i);
        while (message != NULL) {
            switch (message->GetType()){
                case (int) MessageType::GRID:
                    processGridMessage((GridMessage*)message);
                    break;

            }
            client->ReleaseMessage(message);
            message = client->ReceiveMessage(i);
        }
    }
}


void NetworkManager::queueGrid(std::vector<std::vector<uint32_t>> grid_colors){
    if(grid_message) return;
    GridMessage* message = (GridMessage*) client->CreateMessage((int)MessageType::GRID);
    message->client_id = client_id;
    message->grid = grid_colors;
    grid_message = message;
}

void NetworkManager::sendGrid(){
    if(!grid_message) return;
    client->SendMessage((int)GameChannel::RELIABLE, grid_message);
    grid_message = nullptr;
}

std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> NetworkManager::getOpponentsGrid(){
    return opponents_grid;
}

void NetworkManager::update(){
    if(next_cycle > network_clock.getElapsedTime()) return;
    client->AdvanceTime(network_clock.getElapsedTime().asSeconds());
    client->ReceivePackets();

    if(client->IsConnected()){
        processMessages();

        // Sending Messages
        sendGrid();
    }
    client->SendPackets();
    next_cycle += sf::seconds(1.f/TICK_RATE);
};
