#ifndef NETWORK_MANAGER
#define NETWORK_MANAGER

#include <string>
#include <memory>
#include <random>
#include <iostream>

#include "yojimbo.h"
#include "SFML/System.hpp"

#include "connection.hpp"
#include "Definitions.hpp"

enum CONNECTION_STATUS {
    CONNECTED,
    CONNECTING,
    DISCONNECTED,
    ERROR,
    COUNT
};

class NetworkManager{
private: 
    std::shared_ptr<yojimbo::Client> client;
    std::shared_ptr<yojimbo::ClientServerConfig> connection_config;
    uint64_t client_id;
    sf::Clock network_clock;
public:
    NetworkManager();

    CONNECTION_STATUS getConnectionStatus();

    void generateClientID();
    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void processMessages();

    void sendGrid();

    void update();
};

#endif
