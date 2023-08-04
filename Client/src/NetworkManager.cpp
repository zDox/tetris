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

uint64_t NetworkManager::getClientID(){
    return client_id;
}

void NetworkManager::processMessage(yojimbo::Message* message){
    MessageType message_type = convToMessageType(message->GetType());
    if(message_type >= MessageType::COUNT){
        CORE_WARN("NetworkManager - MessageType undefined id: {}", message->GetType());
        return;
    }

    if(!message) {
        CORE_ERROR("NetworkManager - processMessage - Message undefined");
        return;
    }
    if(message_handlers.contains(message_type)){
        message_handlers[message_type](message);
    }
    else{
        CORE_WARN("NetworkManager - processMessage - handler for message with id({}) undefined", message->GetType());
    }    
}

void NetworkManager::processMessages(){
    for (int i = 0; i < connection_config->numChannels; i++) {
        yojimbo::Message* message = client->ReceiveMessage(i);
        while (message != NULL) {
            processMessage(message);
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
    /*
    NETWORK_TRACE("SEND_MESSAGE - PlayerInputMessage - game_id: {}, client_id: {}, frame:  {}\nleft: {}, right: {}, up: {}, down: {}", 
            game_id,
            client->GetClientId(),
            message->player_input.frame,
            message->player_input.left, 
            message->player_input.right, 
            message->player_input.up, 
            message->player_input.down);
    */
}

void NetworkManager::sendMessages(){
    sendPlayerInput();
}

void NetworkManager::registerMessageHandler(MessageType message_type, std::function<void(yojimbo::Message*)> func){
   message_handlers.emplace(message_type, func); 
}


void NetworkManager::update(){
    client->AdvanceTime(network_clock.getElapsedTime().asSeconds());
    client->ReceivePackets();

    if(client->IsConnected()){
        // No need to process messages if nobody can handle them
        if(message_handlers.size() > 0){
            processMessages();
        }

        // Sending Messages
        sendMessages();
    }
    client->SendPackets();
};
