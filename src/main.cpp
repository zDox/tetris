#include "game.hpp"
#include "tetramino.hpp"

int main()
{
    Game game;
    while (game.isRunning())
    {
        game.run();
    }

    game.close();

    return 0;
}
