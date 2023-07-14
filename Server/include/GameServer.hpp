#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <iostream>
#include <time.h>

#include <SFML/System.hpp>
#include "yojimbo.h"

#include "DEFINITIONS.hpp"
#include "connection.hpp"

class GameServer{
private:
    std::shared_ptr<yojimbo::Server> server;
    std::shared_ptr<yojimbo::ConnectionConfig> connection_config;
    bool running = true;
    sf::Time game_time;
public: 
    GameServer();
    
    void processGridMessage(int client_index, yojimbo::Message* message);
    void processMessage(int client_index, yojimbo::Message* message);
    void processMessages();
    void update(sf::Time dt);

    void run();
};

#endif

