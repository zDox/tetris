#ifndef GAME_HPP
#define GAME_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>

#include "GameLogic.hpp"
#include "DEFINITIONS.hpp"
#include "network.hpp"
#include "Player.hpp"

#include <SFML/System.hpp>
#include "yojimbo.h"


typedef std::vector<std::vector<uint32_t>> Grid;

struct ServerPlayer{
    Player player;
    GameLogic gamelogic;
    int tetramino_cursor = -1;
    std::list<PlayerInput> playout_buffer;
};

class Game{
private:
    std::shared_ptr<yojimbo::Server> server;

    int game_id;
    std::vector<TetraminoType> tetramino_sequenz;
    std::unordered_map<uint64_t, std::shared_ptr<ServerPlayer>> players;
    RoundStateType roundstate;

    sf::Clock lobby_clock;
    bool lobby_clock_running = false;
    bool gamelogic_running = false;

    void printGrid(std::vector<std::vector<sf::Color>> grid);
    
    bool needTimeForPlayoutBuffer();
    int getPlayersClientIndex(uint64_t client_id);
    void handleNextTetramino(uint64_t client_id);

    void sendRoundState(uint64_t client_id);
    void sendRoundStates();
    void sendGrid(uint64_t client_id, std::vector<std::vector<sf::Color>> grid);
    void sendPlayerJoin(uint64_t client_id);
    void sendPlayerLeave(uint64_t client_id);


    void updateLobbyState(sf::Time dt);
    void updateIngameState(sf::Time dt);
    void updateEndState(sf::Time dt);
    
public:
    Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id);

    int getGameID();

    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);
    std::shared_ptr<ServerPlayer> getPlayer(uint64_t client_id);
    bool hasPlayer(uint64_t client_id);
    std::unordered_map<uint64_t, std::shared_ptr<ServerPlayer>> getPlayers();

    RoundStateType getRoundState();
    
    void processPlayerInputMessage(uint64_t client_id, PlayerInputMessage* message);

    void update(sf::Time dt);
};

#endif
