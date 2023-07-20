#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <memory>

#include "State.hpp"
#include "Game.hpp"
#include "Tetramino.hpp"
#include "Utils.hpp"
#include "Definitions.hpp"

class GameState : public State{
private:
    std::shared_ptr<GameData> data;

    // SFML
    // SFML -- initUI
    tgui::Label::Ptr points_label;
    tgui::Label::Ptr paused_label;

    // Game Logic                       
    // Game Logic -- Variables
    bool paused = false;
    bool finished = false;
    std::vector<std::vector<sf::RectangleShape>> grid;
    std::shared_ptr<Tetramino> tetra;
    int points = 0;
    bool grid_update_needed = true;

    std::vector<std::vector<sf::RectangleShape>> stationaries;
    bool hold_up = false;
    bool hold_left = false;
    bool hold_right = false;

    // Game Logic -- Timer
    sf::Clock tetra_move_timer;

    // Game Logic -- Functions
    void spawnTetramino();
    bool checkCollisions(int dx, int dy, bool clockwise);
    void lockTetra();

    bool checkLoss();
    void handleLoss();
    
    // Conversion functions
    std::vector<std::vector<uint32_t>> convertGridToColors();

    // Updating
    void resetGrid();
    void putTetraOnGrid();
    void putStationariesOnGrid();
    void updateTetra();
    void checkPoint();
    void updateUI();

    void sendGridData();

    // Drawing
    void drawGrid();
    void drawOpponentGrids();
    void drawUI();
    
    void initWindow();
    void initVariables();
    void initUI();

public:
    GameState(std::shared_ptr<GameData> t_data);
    void init();
    void destroy();

    void handleInputs();
    void update(sf::Time dt);
    void draw();

};

#endif
