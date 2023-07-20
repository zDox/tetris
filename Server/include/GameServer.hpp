#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include <memory>
#include <iostream>
#include <time.h>
#include <queue>

#include <SFML/System.hpp>
#include "yojimbo.h"

#include "DEFINITIONS.hpp"
#include "connection.hpp"
#include "Game.hpp"


struct ServerAdapter;


class GameServer : public std::enable_shared_from_this<GameServer>{
private:
    std::shared_ptr<yojimbo::Server> server;
    std::unique_ptr<yojimbo::ClientServerConfig> connection_config;
    std::unique_ptr<ServerAdapter> adapter;

    std::queue<uint64_t> opponents_queue;

    bool running = true;
    sf::Clock game_clock;
    sf::Time next_cycle;
    std::vector<std::shared_ptr<Game>> games;
    
    std::shared_ptr<Game> getPlayersGame(uint64_t client_id);
    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);
    GameServer() = default;
public: 

    [[nodiscard]] static std::shared_ptr<GameServer> create();
    std::shared_ptr<GameServer> getPtr();
    void init();
    
    void processGridMessage(uint64_t client_index, GridMessage* message);
    void processMessage(int client_index, yojimbo::Message* message);
    void processMessages();
    void sendMessages();
    void update();
    
    // Client Connect/Disconnect Callback from Adapter
    void clientConnected(int client_index);
    void clientDisconnected(int client_index);
    void run();
};
#endif
