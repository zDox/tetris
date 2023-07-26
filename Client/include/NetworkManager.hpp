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

    sf::Clock network_clock;
    sf::Time next_cycle;

    // Queues for Messages to be send
    PlayerCommandType player_command;

    // Game Details
    int game_id = -1;
    std::unordered_map<uint64_t, Player> players;
    RoundStateType roundstate;
    std::queue<TetraminoType> tetramino_queue;
    
    // Processing of Messages funcitons
    void processGridMessage(GridMessage* message);
    void processRoundStateChangeMessage(RoundStateChangeMessage* message);
    void processTetraminoPlacementMessage(TetraminoPlacementMessage* message);
    void processPlayerScoreMessage(PlayerScoreMessage* message);
    void processPlayerJoinMessage(PlayerJoinMessage* message);
    void processPlayerLeaveMessage(PlayerLeaveMessage* message);
    void processMessages();
    
    // Sending of Messages
    void sendPlayerCommands();
    void sendMessages(); 
public:
    NetworkManager();
    
    // Getter and Setter
    ConnectionStatus getConnectionStatus();
    int getGameID();
    RoundStateType getRoundState();
    TetraminoType getNextTetramino();

    void generateClientID();
    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void queuePlayerCommand(PlayerCommandType command_type);
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> getOpponentsGrid();

    void update();
};

#endif
