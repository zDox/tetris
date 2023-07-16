#ifndef GAME_HPP
#define GAME_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <SFML/System.hpp>

#include "DEFINITIONS.hpp"

typedef std::vector<std::vector<uint32_t>> Grid;

struct Player{
    uint64_t client_id;
    Grid grid;
    int points = 0;
    bool lost = false;
};

enum GAMESTATE {
    LOBBY,
    INGAME,
    END,
    COUNT
};

class Game{
private:
    std::unordered_map<uint64_t, Player> players;
    GAMESTATE gamestate;
    sf::Clock lobby_clock;
    bool lobby_clock_running;
public:
    Game();

    void addPlayer(uint64_t client_id);
    void removePlayer(uint64_t client_id);
    bool hasPlayer(uint64_t client_id);

    std::unordered_map<uint64_t, Player> getPlayers();

    void setPlayerPoints(uint64_t client_id, int t_points);
    void setPlayerHasLost(uint64_t client_id);
    void setPlayerGrid(uint64_t client_id, Grid t_grid);

    GAMESTATE getGameState();
    void setGameState(GAMESTATE t_gamestate);

    void updateLobbyState();
    void updateIngameState();
    void updateEndState();
    
    void update();
};

#endif
