#include "GameServer.hpp"

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
    *(connection_config) = GameConnectionConfig();
    server = std::make_shared<yojimbo::Server>(
            yojimbo::GetDefaultAllocator(),
            DEFAULT_PRIVATE_KEY,
            yojimbo::Address(SERVER_ADDRESS, SERVER_PORT),
            *connection_config,
            *adapter,
            0.0);
    if(!server) {
        CORE_ERROR("Failed creating server");
    }
    server->SetLatency(100);
}

std::shared_ptr<GameServer> GameServer::getPtr(){
    return shared_from_this();
}

std::shared_ptr<Game> GameServer::getPlayersGame(uint64_t client_id){
    for(auto [game_id, game] : games){
        if(game->hasPlayer(client_id)){
            return game;
        }
    }
    return nullptr;
}
void GameServer::addPlayer(u_int64_t client_id){
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);
    if(current_game){
        CORE_WARN("Matchmaking - Player({}) is already ingame", client_id);
        return;
    }

    std::shared_ptr<Game> latest_game = nullptr;
    if(next_game_id > 0 && games.contains(next_game_id-1)){
        latest_game = games[next_game_id-1];
    }

    // Check if new game should be created
    if(!latest_game){
        createGame();
    }
    else if (latest_game->getRoundState() != RoundStateType::LOBBY &&
       latest_game->getPlayers().size() >= MAX_PLAYERS){
        createGame();
    }
    latest_game = games[next_game_id-1];
    // Add player to latest game
    latest_game->addPlayer(client_id);
}

void GameServer::removePlayer(u_int64_t client_id){
    std::shared_ptr<Game> current_game = getPlayersGame(client_id);
    if(!current_game) return;
    current_game->removePlayer(client_id);
}

int GameServer::createGame(){ // Returns the game_id of the game it created
    games.emplace(next_game_id, std::make_shared<Game>(server, next_game_id));
    next_game_id++;
    CORE_INFO("Created Game with game_id = {}", next_game_id-1);
    return next_game_id-1;
}


void GameServer::processMessage(int client_index, yojimbo::Message* message){
    uint64_t client_id = server->GetClientId(client_index);
    switch(message->GetType()){
        case (int)MessageType::PLAYER_INPUT:
        {
            PlayerInputMessage* player_input_message = reinterpret_cast<PlayerInputMessage*>(message);
            if(!games.contains(player_input_message->game_id)) return;
            games[player_input_message->game_id]->processPlayerInputMessage(client_id, reinterpret_cast<PlayerInputMessage*>(message));
            break;
        }
        default:
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

void GameServer::updateGames(sf::Time dt){
    for(auto [game_id, game] : games){
        game->update(dt);
    }
}

void GameServer::update(sf::Time dt){
    if(!server->IsRunning()){
        running = false;
        return;
    }
    server->AdvanceTime(game_clock.getElapsedTime().asSeconds());
    server->ReceivePackets();

    processMessages();
    updateGames(dt);

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
    sf::Time last_tick_duration = sf::seconds(0);

    server->Start(MAX_CLIENTS);
 
    while (running){
        if(next_cycle <= game_clock.getElapsedTime()){
            last_tick_duration = game_clock.getElapsedTime() - next_cycle;
            update(last_tick_duration);
            next_cycle += fixed_dt;
        }
        else{
            sf::sleep(next_cycle - game_clock.getElapsedTime());
        }
    }

    server->Stop();
    ShutdownYojimbo();
}
