#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <iostream>
#include <time.h>

#include <SFML/System.hpp>
#include "yojimbo.h"

#include "DEFINITIONS.hpp"
#include "connection.hpp"
#include "Game.hpp"


class GameServer{
private:
    std::shared_ptr<yojimbo::Server> server;
    std::shared_ptr<yojimbo::ConnectionConfig> connection_config;
    bool running = true;
    sf::Clock game_clock;
    sf::Time next_cycle;
    std::vector<std::shared_ptr<Game>> games;
    
    std::shared_ptr<Game> getPlayersGame(uint64_t client_id);
    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);
public: 
    GameServer();
    
    void processGridMessage(int client_index, GridMessage* message);
    void processMessage(int client_index, yojimbo::Message* message);
    void processMessages();
    void update();
    
    // Client Connect/Disconnect Callback from Adapter
    void clientConnected(int client_index);
    void clientDisconnected(int client_index);
    void run();
};

// the adapter
struct ServerAdapter : public yojimbo::Adapter {
private:
    std::shared_ptr<GameServer> game_server;
public:
    explicit ServerAdapter(std::shared_ptr<GameServer> server);

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;
    
    void OnServerClientConnected(int clientIndex) override;
    void OnServerClientDisconnected(int clientIndex) override;
};
#endif


