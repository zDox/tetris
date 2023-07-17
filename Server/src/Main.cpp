#include "GameServer.hpp"

int main(int argc, char* argv[]){
    if(!InitializeYojimbo()){
        std::cerr << "Error: Failed to Initialize yojimbo\n";
        return EXIT_FAILURE;
    }
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_DEBUG);
    std::srand((unsigned int) time(NULL));


    std::shared_ptr<GameServer> game_server = GameServer::create();
    game_server->init();
    game_server->run();
    return EXIT_SUCCESS;
}
