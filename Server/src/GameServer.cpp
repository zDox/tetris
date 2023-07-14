#include "GameServer.hpp"

GameServer::GameServer(){
    server = std::make_shared<yojimbo::Server>(
            yojimbo::GetDefaultAllocator(),
            DEFAULT_PRIVATE_KEY,
            yojimbo::Address(SERVER_ADDRESS, SERVER_PORT),
            game_connection_config,
            adapter,
            0.0);
    connection_config = std::make_shared<yojimbo::ClientServerConfig>();
    *(connection_config) = game_connection_config;
}

void GameServer::processMessage(int client_index, yojimbo::Message* message){
}

void GameServer::processMessages(){
    for(int i=0; i< server->GetNumConnectedClients(); i++){
        if(server->IsClientConnected(i)){
            for(int j=0; j < connection_config->numChannels; j++){
                std::cout << "Running 1\n";
                yojimbo::Message* message = server->ReceiveMessage(i, j);
                std::cout << "Running 2\n";
                while (message != NULL){
                    processMessage(i, message);
                    std::cout << "Running 3\n";
                    server->ReleaseMessage(i, message);
                    std::cout << "Running 4\n";
                    message = server->ReceiveMessage(i, j);
                }
            }
        }
    }
}

void GameServer::update(){
    if(!server->IsRunning()){
        running = false;
        return;
    }
    server->AdvanceTime(game_clock.getElapsedTime().asSeconds());
    server->ReceivePackets();
    processMessages();

    server->SendPackets();
}

void GameServer::run(){
    sf::Time fixed_dt = sf::seconds(1.0f / TICK_RATE);

    server->Start(MAX_CLIENTS);
 
    while (running){
        update();
    }

    server->Stop();
    ShutdownYojimbo();
}
