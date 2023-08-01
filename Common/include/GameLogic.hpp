#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include <vector>
#include <memory>

#include "SFML/Graphics/Color.hpp"

#include "Log.hpp"
#include "Tetramino.hpp"
#include "GameDefinitions.hpp"
#include "Player.hpp"

class GameLogic{
private:
    std::shared_ptr<PlayerInput> player_input;
    int points = 0;
    bool finished = false;
    bool running = false;

    std::shared_ptr<Tetramino> tetra;
    TetraminoType next_tetramino;
    std::vector<std::vector<sf::Color>> grid;
    std::vector<std::vector<sf::Color>> stationaries;

    sf::Time game_time = sf::seconds(0);
    sf::Time next_move_time = sf::seconds(0);

    void spawnTetramino();
    bool checkCollisions(int dx, int dy, bool clockwise);
    void lockTetra();

    bool checkLoss();
    void handleLoss();

     // Updating
    void updateTime(sf::Time dt);
    void resetGrid();
    void putTetraOnGrid();
    void putStationariesOnGrid();
    void updateTetra();
    void checkPoint();

    void initVariables();
public:
    GameLogic();

    bool isRunning();
    bool isFinished();
    void setNextTetramino(TetraminoType tetramino);
    bool isNeedingNextTetramino();
    void setPlayerInput(PlayerInput t_player_input);
    std::vector<std::vector<sf::Color>> getGrid();
    int getPoints();

    void start();
    void init();
    void reset();
    void update(sf::Time dt);
};
#endif
