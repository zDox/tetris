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

struct ClientPlayer{
    Player player;
    std::vector<std::vector<std::shared_ptr<sf::RectangleShape>>> drawing_grid;
    tgui::Label::Ptr main_label;
    tgui::Label::Ptr stats_label;
};

class GameState : public State{
private:
    std::shared_ptr<ApplicationData> data;

    // SFML
    // SFML -- Drawing data
    long frame_counter=0;

    // Game Logic                       
    int game_id = -1;
    uint64_t client_id;
    std::unordered_map<uint64_t, std::shared_ptr<ClientPlayer>> players;
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
    void updatePlayerUI(uint64_t p_client_id);
    void updatePlayerUIs();

    // Drawing
    void drawPlayer(uint64_t p_client_id, int offset_x, int offset_y, float scale);
    void prepareLocalGrid();
    void drawPlayers();
    void drawUI();
    
    void initWindow();
    void initVariables();
    void initHandlers();
    void initUI();

    void initPlayerDrawingGrid(uint64_t p_client_id);
    void initPlayerUI(uint64_t p_client_id);

public:
    explicit GameState(std::shared_ptr<ApplicationData> t_data);
    void init() override;
    void destroy() override;

    // Functions to handle messages
    void handleGridMessage(yojimbo::Message* t_message);
    void handleGameDataMessage(yojimbo::Message* t_message);
    void handleTetraminoPlacementMessage(yojimbo::Message* t_message);
    void handlePlayerDataMessage(yojimbo::Message* t_message);
    void handlePlayerJoinMessage(yojimbo::Message* t_message);
    void handlePlayerLeaveMessage(yojimbo::Message* t_message);

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;

};

#endif
