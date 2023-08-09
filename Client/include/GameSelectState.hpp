#ifndef GAMESELECTSTATE_HPP
#define GAMESELECTSTATE_HPP


#include "GameState.hpp"
#include "State.hpp"
#include "GameDefinitions.hpp"

struct SelectableGame{
    GameData game_data;
    tgui::Panel::Ptr game_panel;
    tgui::Button::Ptr join_button;
    tgui::Label::Ptr game_id_label;
};

class GameSelectState : public State{
private:
    std::shared_ptr<ApplicationData> data;

    std::unordered_map<int, SelectableGame> games; 

    // TGui elements
    tgui::ScrollablePanel::Ptr main_panel;

    void initWindow();
    void initUi();
    void initVariables();
    void initHandlers();

    void joinGame();
public:
    explicit GameSelectState(std::shared_ptr<ApplicationData> t_data);

    void init() override;
    void destroy() override;

    void handleGameJoinResponseMessage(yojimbo::Message* t_message);
    void handleGameDataMessage(yojimbo::Message* t_message);

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;

};
#endif
