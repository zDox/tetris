#ifndef NETWORK_MANAGER
#define NETWORK_MANAGER
// Log needs to be first
#include "Log.hpp"

#include <string>
#include <memory>
#include <random>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <functional>


#include "yojimbo.h"
#include "SFML/System.hpp"

#include "network.hpp"
#include "Definitions.hpp"
#include "Player.hpp"

enum ConnectionStatus {
    CONNECTED,
    CONNECTING,
    DISCONNECTED,
    ERROR,
};

class NetworkManager{
private: 
    std::shared_ptr<yojimbo::Client> client;
    std::shared_ptr<yojimbo::ClientServerConfig> connection_config;
    ClientAdapter adapter;
    uint64_t client_id;
    std::unordered_map<MessageType, std::function<void(yojimbo::Message*)>> message_handlers;
    int game_id;

    sf::Clock network_clock;
    sf::Time next_cycle;

    // Queues for Messages to be send
    std::shared_ptr<PlayerInput> player_input;
    
    void processMessage(yojimbo::Message* message);
    void processMessages();
    
    // Sending of Messages
    void sendPlayerInput();
    void sendMessages(); 
public:
    NetworkManager();
    
    // Getter and Setter
    ConnectionStatus getConnectionStatus();

    void generateClientID();
    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void registerMessageHandler(MessageType message_type, std::function<void(yojimbo::Message*)> func);

    void queuePlayerInput(PlayerInput t_player_input);

    void update();
};

#endif
