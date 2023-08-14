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

enum class ConnectionStatus {
    CONNECTED,
    CONNECTING,
    DISCONNECTED,
    ERROR_CONNECTION
};

class NetworkManager{
private: 
    std::shared_ptr<yojimbo::Client> client;
    std::shared_ptr<yojimbo::ClientServerConfig> connection_config;
    ClientAdapter adapter;
    uint64_t client_id;
    std::unordered_map<MessageType, std::function<void(yojimbo::Message*)>> message_handlers;
    std::function<void(ConnectionStatus)> connection_status_handler;
    bool running;

    sf::Clock network_clock;
    sf::Time next_cycle;
    unsigned long cycle_count = 0;

    // Queues for Messages to be send
    int game_id;
    std::shared_ptr<PlayerInput> player_input;
    std::string requested_username = "";
    int wanted_game_id = -1;
    bool should_send_gamelist_request = false;
    
    void generateClientID();

    void processMessage(yojimbo::Message* message);
    void processMessages();
    
    // Sending of Messages
    void sendPlayerInput();
    void sendLoginRequest();
    void sendGameListRequest();
    void sendGameJoinRequest();
    void sendMessages(); 
public:
    NetworkManager();
    
    // Getter and Setter
    ConnectionStatus getConnectionStatus();
    uint64_t getClientID();

    
    bool init();
    void destroy();

    void connect(std::string);
    void disconnect();

    void start();
    void stop();

    void setGameID(int t_game_id);

    void registerConnectionStatusHandler(std::function<void(ConnectionStatus)> func);
    void registerMessageHandler(MessageType message_type, std::function<void(yojimbo::Message*)> func);
    void unregisterMessageHandlers();
    void unregisterConnectionStatusHandler();

    void queuePlayerInput(PlayerInput t_player_input);
    void queueLoginRequest(std::string username);
    void queueGameListRequest();
    void queueGameJoinRequest(int t_game_id);

    void update();
};

#endif
