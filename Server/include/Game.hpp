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


using Grid = std::vector<std::vector<uint32_t>>;

struct GamePlayer{
    Player player;
    GameLogic gamelogic;
    int tetramino_cursor = -1;
    std::list<PlayerInput> playout_buffer;
};

inline bool compServerPlayer(const std::shared_ptr<GamePlayer> &l, const std::shared_ptr<GamePlayer> &r){
    if(l->player.points == r->player.points) return l->gamelogic.isFinished() < r->gamelogic.isFinished();
    return l->player.points > r->player.points;
}

class Game{
private:
    std::shared_ptr<yojimbo::Server> server;

    int game_id;
    int min_players;
    int max_players;
    std::vector<TetraminoType> tetramino_sequenz;
    std::unordered_map<uint64_t, std::shared_ptr<GamePlayer>> players;
    RoundStateType roundstate;

    sf::Clock lobby_clock;
    bool lobby_clock_running = false;
    sf::Clock end_clock;
    bool end_clock_running = false;

    bool gamelogic_running = false;

    void printGrid(std::vector<std::vector<sf::Color>> grid);
    
    bool isFinished();
    bool needTimeForPlayoutBuffer();
    bool positionsHaveChanged();
    int getPlayersClientIndex(uint64_t client_id);
    void handleNextTetramino(uint64_t client_id);
    void handleGameFinished();

    void sendGameData(uint64_t client_id);
    void broadcastGameData();
    void sendGrid(uint64_t sender_id, uint64_t receiver_id);
    void broadcastGrid(uint64_t client_id);
    void sendPlayerJoin(uint64_t sender_id, uint64_t receiver_id);
    void broadcastPlayerJoin(uint64_t client_id);
    void broadcastPlayerLeave(uint64_t client_id);
    void sendPlayerData(uint64_t sender_id, uint64_t receiver_id);
    void broadcastPlayerData(uint64_t client_id);


    void updateLobbyState(sf::Time dt);
    void updateIngameState(sf::Time dt);
    void updateEndState(sf::Time dt);
    
public:
    Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id, int t_min_players, int t_max_players);

    int getGameID();
    GameData getGameData();

    int getMinPlayers();
    int getMaxPlayers();
    void addPlayer(Player t_player);
    void removePlayer(uint64_t client_id);
    std::shared_ptr<GamePlayer> getPlayer(uint64_t client_id);
    bool hasPlayer(uint64_t client_id);
    std::unordered_map<uint64_t, std::shared_ptr<GamePlayer>> getPlayers();
    bool isFull();

    RoundStateType getRoundState();
    
    void processPlayerInput(uint64_t client_id, PlayerInputMessage* message);

    void update(sf::Time dt);
};

#endif
