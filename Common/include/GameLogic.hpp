#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include <vector>
#include <memory>

#include "SFML/Graphics/Color.hpp"

#include "Tetramino.hpp"

class GameLogic{
private:
    int points = 0;
    bool finished = false;

    std::shared_ptr<Tetramino> tetra;
    std::vector<std::vector<sf::Color>> grid;
    std::vector<std::vector<sf::Color>> stationaries;

    sf::Clock tetra_move_timer;

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

    void init();
    void reset();
    void update(sf::Time dt);
};
#endif
