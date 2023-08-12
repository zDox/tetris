#include "Log.hpp"
#include "Game.hpp"

int main()
{
    CORE_INFO("Starting Client");
    Game game;
    if(!game.init()) return EXIT_FAILURE;
    game.run();
    CORE_INFO("Closing Client");
    game.close();
    return EXIT_SUCCESS;
}
