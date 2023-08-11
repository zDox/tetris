#include "Log.hpp"
#include "GameServer.hpp"

int main(int argc, char* argv[]){
    Log::init();
    if(!InitializeYojimbo()){
        CORE_ERROR("Libs -  Failed to Initialize yojimbo");
        return EXIT_FAILURE;
    }
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_DEBUG);
    std::srand((unsigned int) time(NULL));

    std::shared_ptr<GameServer> game_server = GameServer::create();
    game_server->init();
    game_server->run();
    return EXIT_SUCCESS;
}
