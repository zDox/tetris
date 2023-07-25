#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include <vector>
#include <memory>

#include "SFML/Graphics/Color.hpp"

#include "Tetramino.hpp"
#include "GameDefinitions.hpp"
#include "Command.hpp"

class GameLogic{
private:
    PlayerCommandType player_command;
    int points = 0;
    bool finished = false;
    bool running = false;

    std::shared_ptr<Tetramino> tetra;
    std::shared_ptr<TetraminoType> next_tetramino;
    std::vector<std::vector<sf::Color>> grid;
    std::vector<std::vector<sf::Color>> stationaries;

    sf::Clock tetra_move_timer;
    sf::Time next_move_time;

    void spawnTetramino();
    bool checkCollisions(int dx, int dy, bool clockwise);
    void lockTetra();

    bool checkLoss();
    void handleLoss();

     // Updating
    void resetGrid();
    void putTetraOnGrid();
    void putStationariesOnGrid();
    void updateTetra();
    void checkPoint();

    void initVariables();
public:
    GameLogic();

    bool isRunning();
    void setNextTetramino(TetraminoType tetramino);
    bool isNeedingNextTetramino();
    void setPlayerCommand(PlayerCommandType t_player_command);
    std::vector<std::vector<sf::Color>> getGrid();
    int getPoints();

    void start();
    void init();
    void reset();
    void update(sf::Time dt);
};
#endif
