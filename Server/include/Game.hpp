#ifndef GAME_HPP
#define GAME_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>

#include <SFML/System.hpp>
#include "yojimbo.h"

#include "DEFINITIONS.hpp"
#include "network.hpp"
#include "Player.hpp"

typedef std::vector<std::vector<uint32_t>> Grid;


class Game{
private:
    std::shared_ptr<yojimbo::Server> server;

    int game_id;
    std::unordered_map<uint64_t, std::shared_ptr<Player>> players;
    RoundStateType roundstate;

    sf::Clock lobby_clock;
    bool lobby_clock_running; 
    
    int getPlayersClientIndex(uint64_t client_id);

    void sendRoundState(uint64_t client_id);
    void sendRoundStates();


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

    RoundStateType getRoundState();
    
    void processGridMessage(uint64_t client_id, GridMessage* message);

    void update(sf::Time dt);
};

#endif
