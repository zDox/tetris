#include "Game.hpp"

Game::Game(){
    gamestate = GAMESTATE::LOBBY;
}

void Game::addPlayer(uint64_t client_id){
    if(players.contains(client_id)) return;
    Player new_player;
    new_player.client_id = client_id;
    players[client_id] = new_player;
    if(players.size() >= MIN_STARTING_PLAYERS && !lobby_clock_running){
        lobby_clock_running = true;
        lobby_clock.restart();
    }
}


void Game::removePlayer(uint64_t client_id){
    if(!players.contains(client_id)) return;
    players.erase(client_id);
    if(gamestate == GAMESTATE::INGAME && players.size() < MIN_INGAME_PLAYERS){
        setGameState(GAMESTATE::END);
    }
    else if(gamestate == GAMESTATE::LOBBY && players.size() < MIN_STARTING_PLAYERS){
        lobby_clock_running = false;
    }
}

bool Game::hasPlayer(uint64_t client_id){
    return players.contains(client_id);
}

std::unordered_map<uint64_t, Player> Game::getPlayers(){
    return players;
}
    
void Game::setPlayerPoints(uint64_t client_id, int t_points){
    if(!players.contains(client_id)) return;
    players[client_id].points = t_points;
}

void Game::setPlayerHasLost(uint64_t client_id){
    if(!players.contains(client_id)) return;
    players[client_id].lost = true;
}

void Game::setPlayerGrid(uint64_t client_id, Grid t_grid){
    if(!players.contains(client_id)) return;
    players[client_id].grid = t_grid;
}

GAMESTATE Game::getGameState(){
    return gamestate;
}

void Game::setGameState(GAMESTATE t_gamestate){
    gamestate = t_gamestate;
}

void Game::updateLobbyState(){
    if(lobby_clock.getElapsedTime() >= sf::seconds(LOBBY_WAIT_TIME)){
        if(players.size() < MIN_STARTING_PLAYERS) {
            return;
        }
        setGameState(GAMESTATE::INGAME);
        return;
    }
}

void Game::updateIngameState(){
    
}

void Game::updateEndState(){
}

void Game::update(){
    switch(gamestate){
        case GAMESTATE::LOBBY:
           updateLobbyState();
           break;
        case GAMESTATE::INGAME:
           updateIngameState();
           break;
        case GAMESTATE::END:
           updateEndState();
           break;
        default:
           return;
    }
}
