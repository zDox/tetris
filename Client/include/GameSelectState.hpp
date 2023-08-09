#ifndef GAMESELECTSTATE_HPP
#define GAMESELECTSTATE_HPP


#include "GameState.hpp"
#include "State.hpp"
#include "GameDefinitions.hpp"

struct SelectableGame{
    GameData game_data;
    tgui::Panel::Ptr game_panel;
    tgui::Label::Ptr heading;

    tgui::Label::Ptr status_label;
    tgui::Label::Ptr status_text;

    tgui::Label::Ptr players_label;
    tgui::Label::Ptr players_text;

    tgui::Button::Ptr join_button;
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

    void joinGame(int game_id);
public:
    explicit GameSelectState(std::shared_ptr<ApplicationData> t_data);

    void init() override;
    void destroy() override;

    void handleGameJoinResponseMessage(yojimbo::Message* t_message);
    void handleGameDataMessage(yojimbo::Message* t_message);

    void updateGameUIs();

    void updateUI();

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;

};
#endif
