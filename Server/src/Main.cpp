#include "GameServer.hpp"

int main(int argc, char* argv[]){
    if(!InitializeYojimbo()){
        std::cerr << "Error: Failed to Initialize yojimbo\n";
        return EXIT_FAILURE;
    }
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
    std::srand((unsigned int) time(NULL));

    GameServer game_server;
    game_server.run();
    return EXIT_SUCCESS;
}
