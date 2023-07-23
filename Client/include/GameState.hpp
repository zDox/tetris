#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <memory>

#include "State.hpp"
#include "Game.hpp"
#include "Tetramino.hpp"
#include "Utils.hpp"
#include "Definitions.hpp"
#include "Command.hpp"

class GameState : public State{
private:
    std::shared_ptr<GameData> data;

    // SFML
    // SFML -- initUI
    tgui::Label::Ptr points_label;
    tgui::Label::Ptr paused_label;

    // Game Logic                       
    // Game Logic -- Variables

    bool hold_up = false;
    bool hold_left = false;
    bool hold_right = false;

    // Game Logic -- Timer

    void updateUI();

    void sendPlayerCommand(PlayerCommandType command);

    // Drawing
    void drawGrid();
    void drawOpponentGrids();
    void drawUI();
    
    void initWindow();
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
