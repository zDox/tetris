#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <memory>
#include <unordered_map>
#include <functional>

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
    int game_id = -1;
    uint64_t client_id;
    std::unordered_map<uint64_t, Player> players;
    RoundStateType roundstate;
    std::queue<TetraminoType> tetramino_queue;
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
    void initHandlers();
    void initUI();

public:
    explicit GameState(std::shared_ptr<GameData> t_data);
    void init() override;
    void destroy() override;

    // Functions to handle messages
    void handleGridMessage(yojimbo::Message* t_message);
    void handleRoundStateChangeMessage(yojimbo::Message* t_message);
    void handleTetraminoPlacementMessage(yojimbo::Message* t_message);
    void handlePlayerScoreMessage(yojimbo::Message* t_message);
    void handlePlayerJoinMessage(yojimbo::Message* t_message);
    void handlePlayerLeaveMessage(yojimbo::Message* t_message);

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;

};

#endif
