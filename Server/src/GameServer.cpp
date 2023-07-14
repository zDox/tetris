#include "GameServer.hpp"

GameServer::GameServer(){
    server = std::make_shared<yojimbo::Server>(
            yojimbo::GetDefaultAllocator(),
            DEFAULT_PRIVATE_KEY,
            yojimbo::Address(SERVER_ADDRESS, SERVER_PORT),
            config,
            adapter,
            0.0);
}

void GameServer::processMessage(int client_index, yojimbo::Message* message){
}

void GameServer::processMessages(){
    for(int i=0; i< MAX_CLIENTS; i++){
        if(server->IsClientConnected(i)){
            for(int j=0; j < connection_config->numChannels; j++){
                yojimbo::Message* message = server->ReceiveMessage(i, j);
                while (message != NULL){
                    processMessage(i, message);
                    server->ReleaseMessage(i, message);
                }
            }
        }
    }
}

void GameServer::update(sf::Time dt){
    if(!server->IsRunning()){
        running = false;
        return;
    }
    server->AdvanceTime(game_time.asMilliseconds());
    server->ReceivePackets();
    processMessages();

    server->SendPackets();
}

void GameServer::run(){
    sf::Time fixed_dt = sf::seconds(1.0f / TICK_RATE);
    sf::Clock game_clock;

    server->Start(MAX_CLIENTS);
 
    while (running){
        if(game_clock.getElapsedTime() >= fixed_dt){
            game_time += game_clock.getElapsedTime();
            game_clock.restart();
            update(fixed_dt);
        }
        else{
            sf::sleep(fixed_dt - game_clock.getElapsedTime());
        }
    }

    server->Stop();
    ShutdownYojimbo();
}
