#ifndef GAME_HPP
#define GAME_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>

#include <SFML/System.hpp>
#include "yojimbo.h"

#include "DEFINITIONS.hpp"
#include "connection.hpp"

typedef std::vector<std::vector<uint32_t>> Grid;

struct Player{
    uint64_t client_id;
    Grid grid;
    int points = 0;
    bool lost = false;
};

class Game{
private:
    std::shared_ptr<yojimbo::Server> server;

    int game_id;
    std::unordered_map<uint64_t, std::shared_ptr<Player>> players;
    GameState gamestate;

    sf::Clock lobby_clock;
    bool lobby_clock_running; 
    
    int getPlayersClientIndex(uint64_t client_id);

    void sendGameState(uint64_t client_id);
    void sendGameStates();


    void updateLobbyState(sf::Time dt);
    void updateIngameState(sf::Time dt);
    void updateEndState(sf::Time dt);
    
public:
    Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id);

    int getGameID();

    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);
    std::shared_ptr<Player> getPlayer(uint64_t client_id);
    bool hasPlayer(uint64_t client_id);
    std::unordered_map<uint64_t, std::shared_ptr<Player>> getPlayers();

    GameState getGameState();
    
    void processGridMessage(uint64_t client_id, GridMessage* message);

    void update(sf::Time dt);
};

#endif
