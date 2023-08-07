#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include <memory>
#include <iostream>
#include <time.h>
#include <queue>

#include "Game.hpp"
#include "DEFINITIONS.hpp"
#include "network.hpp"

#include <SFML/System.hpp>
#include "yojimbo.h"



struct ServerAdapter;


class GameServer : public std::enable_shared_from_this<GameServer>{
private:
    std::shared_ptr<yojimbo::Server> server;
    std::unique_ptr<yojimbo::ClientServerConfig> connection_config;
    std::unique_ptr<ServerAdapter> adapter;

    std::queue<uint64_t> opponents_queue;


    bool running = true;
    sf::Clock game_clock;
    sf::Time next_cycle = sf::seconds(0);
    std::unordered_map<int, std::shared_ptr<Game>> games;
    int next_game_id = 0;
    
    std::shared_ptr<Game> getPlayersGame(uint64_t client_id);
    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);

    int createGame();

    void updateGames(sf::Time dt);

    void processLoginRequest(LoginRequestMessage* message);

    void processMessage(int client_index, yojimbo::Message* message);
    void processMessages();
    void sendMessages();
    void update(sf::Time dt);
    
    GameServer() = default;
public: 

    [[nodiscard]] static std::shared_ptr<GameServer> create();
    std::shared_ptr<GameServer> getPtr();
    void init();
    
    // Client Connect/Disconnect Callback from Adapter
    void clientConnected(int client_index);
    void clientDisconnected(int client_index);
    void run();
};
#endif
