#include "GameServer.hpp"
#include "connection.hpp"

struct ServerAdapter : yojimbo::Adapter{
private:
    std::weak_ptr<GameServer> game_server;
public: 
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator){
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }

    ServerAdapter(std::shared_ptr<GameServer> t_game_server) : game_server(t_game_server){};

    void OnServerClientConnected(int clientIndex){
        if (!game_server.expired()) {
            std::shared_ptr<GameServer> g = game_server.lock();
            g->clientConnected(clientIndex);
        }
    }
    void OnServerClientDisconnected(int clientIndex){
        if (!game_server.expired()) {
            std::shared_ptr<GameServer> g = game_server.lock();
            g->clientDisconnected(clientIndex);
        }
    }
};



// GameServer implementation


// No public constructor, only a factory function,
// so there's no way to have getptr return nullptr.
[[nodiscard]] std::shared_ptr<GameServer> GameServer::create()
{
    // Not using std::make_shared<Best> because the c'tor is private.
    return std::shared_ptr<GameServer>(new GameServer());
}

void GameServer::init(){
    adapter = std::make_unique<ServerAdapter>(this->getPtr());
    connection_config = std::make_unique<yojimbo::ClientServerConfig>();
    *(connection_config) = game_connection_config;
    server = std::make_shared<yojimbo::Server>(
            yojimbo::GetDefaultAllocator(),
            DEFAULT_PRIVATE_KEY,
            yojimbo::Address(SERVER_ADDRESS, SERVER_PORT),
            *connection_config,
            *adapter,
            0.0);
    if(!server) {
        std::cerr << "Failed creating server\n";
    }
}

std::shared_ptr<GameServer> GameServer::getPtr(){
    return shared_from_this();
}

std::shared_ptr<Game> GameServer::getPlayersGame(uint64_t client_id){
    for(auto game : games){
        if(game->hasPlayer(client_id)){
            return game;
        }
    }
    return nullptr;
}
void GameServer::addPlayer(u_int64_t client_id){
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);
    if(current_game){
        std::cout << "Player: " << client_id << " is already ingame\n";
        return;
    }
    // Check if new game should be created
    if(games.size() == 0){
        games.push_back(std::make_shared<Game>());
    }
    else if (games[games.size()-1]->getGameState() != GAMESTATE::LOBBY &&
       games[games.size()-1]->getPlayers().size() >= MAX_PLAYERS){
        games.push_back(std::make_shared<Game>());
    }
    // Add player to latest game
    games[games.size()-1]->addPlayer(client_id);
}

void GameServer::removePlayer(u_int64_t client_id){
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);
    if(!current_game) return;
    current_game->removePlayer(client_id);
}


void GameServer::processGridMessage(uint64_t client_id, GridMessage* grid_message){
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);

    std::vector<std::vector<uint32_t>> old_grid = current_game->getPlayer(client_id)->grid;
    std::vector<std::vector<uint32_t>> o_grid = grid_message->grid;

    for(auto[rec_client_id, rec_player] : current_game->getPlayers()){
        int rec_index;
        for(rec_index = 0; rec_index<server->GetNumConnectedClients(); rec_index++){
            if(server->GetClientId(rec_index) == rec_client_id) break;
        
        }
        GridMessage* response = (GridMessage*) server->CreateMessage(rec_index, (int)MessageType::GRID);
        response->client_id = client_id;
        response->grid = o_grid;
        server->SendMessage(rec_index, (int)GameChannel::RELIABLE, response);
    }
}

void GameServer::processMessage(int client_index, yojimbo::Message* message){
    uint64_t client_id = server->GetClientId(client_index);
    switch(message->GetType()){
        case (int)MessageType::GRID:
            processGridMessage(client_id, reinterpret_cast<GridMessage*>(message));
            break;
        defaul:
            break;
    }
}

void GameServer::processMessages(){
    for(int i=0; i< server->GetNumConnectedClients(); i++){
        if(server->IsClientConnected(i)){
            for(int j=0; j < connection_config->numChannels; j++){
                yojimbo::Message* message = server->ReceiveMessage(i, j);
                while (message != NULL){
                    processMessage(i, message);
                    server->ReleaseMessage(i, message);
                    message = server->ReceiveMessage(i, j);
                }
            }
        }
    }
}

void GameServer::sendMessages(){
}

void GameServer::update(){
    if(!server->IsRunning()){
        running = false;
        return;
    }
    server->AdvanceTime(game_clock.getElapsedTime().asSeconds());
    server->ReceivePackets();
    processMessages();
    /*
    for(int i=0; i< server->GetNumConnectedClients(); i++){
        if(server->IsClientConnected(i)){
            for(int j=0; j < connection_config->numChannels; j++){
                yojimbo::Message* message = server->ReceiveMessage(i, j);
                while (message != NULL){
                    switch (message->GetType()){
                        case (int)MessageType::GRID:
                            GridMessage* grid_message = static_cast<GridMessage*>(message);
                            uint64_t client_id = server->GetClientId(i);
                            processGridMessage(client_id, grid_message); 
                            break;
                    }

                    server->ReleaseMessage(i, message);
                    message = server->ReceiveMessage(i, j);
                }
            }
        }
    }
    */
    
    server->SendPackets();
}

void GameServer::clientConnected(int client_index){
    uint64_t client_id = server->GetClientId(client_index);
    addPlayer(client_id);
}

void GameServer::clientDisconnected(int client_index){
    uint64_t client_id = server->GetClientId(client_index);
    removePlayer(client_id);
}

void GameServer::run(){
    sf::Time fixed_dt = sf::seconds(1.0f / TICK_RATE);

    server->Start(MAX_CLIENTS);
 
    while (running){
        if(next_cycle <= game_clock.getElapsedTime()){
            update();
            next_cycle += fixed_dt;
        }
        else{
            sf::sleep(next_cycle - game_clock.getElapsedTime());
        }
    }

    server->Stop();
    ShutdownYojimbo();
}
