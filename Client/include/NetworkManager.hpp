#ifndef NETWORK_MANAGER
#define NETWORK_MANAGER

#include <string>
#include <memory>
#include <random>
#include <iostream>
#include <unordered_map>

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
    ClientAdapter adapter;
    uint64_t client_id;

    sf::Clock network_clock;
    sf::Time next_cycle;

    GridMessage* grid_message = nullptr;
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> opponents_grid;
public:
    NetworkManager();

    CONNECTION_STATUS getConnectionStatus();

    void generateClientID();
    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void processGridMessage(GridMessage* message);
    void processMessages();
    
    void queueGrid(std::vector<std::vector<uint32_t>> grid_colors);
    void sendGrid();
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> getOpponentsGrid();

    void update();
};

#endif
