#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <memory>

#include "State.hpp"
#include "Game.hpp"
#include "Definitions.hpp"
#include "GameDefinitions.hpp"
#include "Command.hpp"
#include "GameLogic.hpp"

class GameState : public State{
private:
    std::shared_ptr<GameData> data;

    // SFML
    // SFML -- initUI
    tgui::Label::Ptr points_label;
    tgui::Label::Ptr paused_label;

    // SFML -- Drawing data
    std::vector<std::vector<sf::RectangleShape>> drawing_grid;

    // Game Logic                       
    GameLogic game_logic;
    bool hold_up = false;
    bool hold_left = false;
    bool hold_right = false;


    void handleKeyboard();
    void updateUI();

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
