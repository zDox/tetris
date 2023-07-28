#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <memory>

#include "State.hpp"
#include "Game.hpp"
#include "Definitions.hpp"
#include "GameDefinitions.hpp"
#include "Player.hpp"
#include "GameLogic.hpp"
#include "LoginState.hpp"

class GameState : public State{
private:
    std::shared_ptr<GameData> data;

    // SFML
    // SFML -- initUI
    tgui::Label::Ptr points_label;
    tgui::Label::Ptr paused_label;

    // SFML -- Drawing data
    std::vector<std::vector<sf::RectangleShape>> drawing_grid;
    long frame_counter=0;

    // Game Logic                       
    GameLogic game_logic;
    bool hold_up = false;
    bool hold_left = false;
    bool hold_right = false;
    sf::Clock game_clock;
    sf::Time next_cycle = sf::seconds(0);


    void handleNextTetramino();
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
