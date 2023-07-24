#include "Game.hpp"

Game::Game(std::shared_ptr<yojimbo::Server> t_server, int t_game_id) : server(t_server), game_id(t_game_id){
    roundstate = RoundStateType::LOBBY;
}

void Game::addPlayer(uint64_t client_id){
    if(players.contains(client_id)) return;
    std::shared_ptr<ServerPlayer> new_player = std::make_shared<ServerPlayer>();
    new_player->player.client_id = client_id;
    players[client_id] = new_player;
    players[client_id]->gamelogic.init();


    std::cout << std::to_string(game_id) << " Player (" << std::to_string(client_id) << ") joined the game.\n";
    if(players.size() >= MIN_STARTING_PLAYERS && !lobby_clock_running){
        lobby_clock_running = true;
        lobby_clock.restart();
        std::cout << std::to_string(game_id) << " Lobby countdown is has started.\n";
    }
    sendRoundState(client_id);
}


void Game::removePlayer(uint64_t client_id){
    if(!players.contains(client_id)) return;
    players.erase(client_id);

    std::cout << std::to_string(game_id) << " Player (" << std::to_string(client_id) << ") leaved the game.\n";
    if(roundstate == RoundStateType::INGAME && players.size() < MIN_INGAME_PLAYERS){
        roundstate = RoundStateType::END;
        sendRoundStates();
    }
    else if(roundstate == RoundStateType::LOBBY && players.size() < MIN_STARTING_PLAYERS){
        lobby_clock_running = false;
        sendRoundStates();
    }
}

bool Game::hasPlayer(uint64_t client_id){
    return players.contains(client_id);
}

std::shared_ptr<ServerPlayer> Game::getPlayer(uint64_t client_id){
    if(players.contains(client_id)){
        return players[client_id];
    }
    return nullptr;
}

int Game::getPlayersClientIndex(uint64_t client_id){
    int i;
    for(i = 0; i < server->GetNumConnectedClients(); i++){
        if(server->GetClientId(i) == client_id) break;
    }
    return i;
}

std::unordered_map<uint64_t, std::shared_ptr<ServerPlayer>> Game::getPlayers(){
    return players;
}

RoundStateType Game::getRoundState(){
    return roundstate;
}

void Game::processPlayerCommandMessage(uint64_t client_id, PlayerCommandMessage* message){
    if(!players.contains(client_id)) return;
    if(game_id != message->game_id) return;

    players[client_id]->gamelogic.setPlayerCommand(message->command_type);
}

void Game::sendRoundState(uint64_t client_id){
    int client_index = getPlayersClientIndex(client_id);
    RoundStateChangeMessage* message = (RoundStateChangeMessage*) server->CreateMessage(client_index, (int)MessageType::ROUNDSTATECHANGE);
    message->roundstate = roundstate;
    server->SendMessage(client_index, (int) GameChannel::RELIABLE, message);
}

void Game::sendRoundStates(){
    for(auto [p_client_id, player] : players){
        sendRoundState(p_client_id);
    }
}

void Game::updateLobbyState(sf::Time dt){
    if(lobby_clock.getElapsedTime() >= sf::seconds(LOBBY_WAIT_TIME)){
        if(players.size() < MIN_STARTING_PLAYERS) {
            return;
        }
        roundstate = RoundStateType::INGAME;
        sendRoundStates(); 
        return;
    }
}

void Game::updateIngameState(sf::Time dt){
    for(auto [client_id, player] : players){
        players[client_id]->gamelogic.update(dt);
    }
}

void Game::updateEndState(sf::Time dt){
}

void Game::update(sf::Time dt){
    switch(roundstate){
        case RoundStateType::LOBBY:
           updateLobbyState(dt);
           break;
        case RoundStateType::INGAME:
           updateIngameState(dt);
           break;
        case RoundStateType::END:
           updateEndState(dt);
           break;
        default:
           return;
    }
}
