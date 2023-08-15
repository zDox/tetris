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
            return ConnectionStatus::ERROR_CONNECTION;
    }
    return ConnectionStatus::ERROR_CONNECTION;
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
    yojimbo::Address server_address(text_address.c_str());
    if(!server_address.IsValid()) return;
    if(server_address.GetPort() == 0) server_address.SetPort(SERVER_PORT);

    client->InsecureConnect(DEFAULT_PRIVATE_KEY, client_id, server_address);
} 

void NetworkManager::disconnect(){
    client->Disconnect();
}

void NetworkManager::start(){
    running = true;
}

void NetworkManager::stop(){
    running = false;
}

void NetworkManager::setGameID(int t_game_id){
    game_id = t_game_id;
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
            if(!running) return;
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

void NetworkManager::queueLoginRequest(std::string username){
    requested_username = username;
}

void NetworkManager::queueGameListRequest(){
    should_send_gamelist_request = true;
}

void NetworkManager::queueGameJoinRequest(int t_game_id){
    wanted_game_id = t_game_id;
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

void NetworkManager::sendLoginRequest(){
    if(requested_username == "")return;

    CORE_TRACE("NetworkManager - Sending LoginRequest - requested username: {}", requested_username); 
    LoginRequestMessage* message = (LoginRequestMessage*) client->CreateMessage((int)MessageType::LOGIN_REQUEST);
    message->username = requested_username;
    client->SendMessage((int)GameChannel::RELIABLE, message);
    requested_username = "";
}

void NetworkManager::sendGameListRequest(){
    if(!should_send_gamelist_request) return;

    CORE_TRACE("NetworkManager - Sending GameListRequest");
    GameListRequestMessage* message = (GameListRequestMessage*) client->CreateMessage((int)MessageType::GAME_LIST_REQUEST);
    client->SendMessage((int)GameChannel::RELIABLE, message);
    should_send_gamelist_request = false;
}

void NetworkManager::sendGameJoinRequest(){
    if(wanted_game_id == -1) return;

    CORE_TRACE("NetworkManager - Sending GameJoinRequest - wanted_game_id: {}", wanted_game_id);
    GameJoinRequestMessage* message = (GameJoinRequestMessage*) client->CreateMessage((int)MessageType::GAME_JOIN_REQUEST);
    message->game_id = wanted_game_id;
    client->SendMessage((int)GameChannel::RELIABLE, message);
    wanted_game_id = -1;
}

void NetworkManager::sendMessages(){
    sendPlayerInput();
    sendLoginRequest();
    sendGameListRequest();
    sendGameJoinRequest();
}

void NetworkManager::registerConnectionStatusHandler(std::function<void(ConnectionStatus)> func){
    connection_status_handler = func;
}

void NetworkManager::registerMessageHandler(MessageType message_type, std::function<void(yojimbo::Message*)> func){
   message_handlers.emplace(message_type, func); 
}

void NetworkManager::unregisterMessageHandlers(){
    message_handlers.clear();
    CORE_TRACE("NetworkManager - Size of message handlers: {}", message_handlers.size());
}

void NetworkManager::unregisterConnectionStatusHandler(){
    connection_status_handler = nullptr;
}

void NetworkManager::update(){
    ConnectionStatus before_state = getConnectionStatus();
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
    cycle_count++;

    // handle Client state change
    ConnectionStatus after_state = getConnectionStatus();
    if(after_state != before_state && connection_status_handler) connection_status_handler(after_state);
};
