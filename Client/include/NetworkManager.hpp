#ifndef NETWORK_MANAGER
#define NETWORK_MANAGER

#include <string>
#include <memory>
#include <random>
#include <iostream>
#include <unordered_map>
#include <queue>

#include "yojimbo.h"
#include "SFML/System.hpp"

#include "network.hpp"
#include "Definitions.hpp"

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

    sf::Clock network_clock;
    sf::Time next_cycle;

    std::queue<PlayerCommandType> player_command_queue;

    int game_id = -1;
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> opponents_grid;
public:
    NetworkManager();

    ConnectionStatus getConnectionStatus();

    void generateClientID();
    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void processGridMessage(GridMessage* message);
    void processTetraminoPlacement(TetraminoPlacementMessage* message);
    void processMessages();

    void sendPlayerCommands();
    void sendMessages();
    
    int getGameID();

    void queuePlayerCommand(PlayerCommandType command_type);
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> getOpponentsGrid();

    void update();
};

#endif
