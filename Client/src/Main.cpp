#include "Game.hpp"

int main()
{
    Game game;
    if(!game.init()) return EXIT_FAILURE;
    game.run();
    game.close();
    return EXIT_SUCCESS;
}
