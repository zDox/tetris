#ifndef LOGINSTATE_HPP
#define LOGINSTATE_HPP

#include <memory>
#include <string>

#include "GameState.hpp"
#include "State.hpp"
#include "Game.hpp"
#include "GameDefinitions.hpp"

class LoginState : public State{
private:
    std::shared_ptr<GameData> data;

    // TGui elements
    tgui::Panel::Ptr panel;
    tgui::Label::Ptr heading;
    tgui::Label::Ptr label_ipaddress;
    tgui::EditBox::Ptr box_ipaddress;
    tgui::Button::Ptr button_connect;

    void login();

    void initWindow();
    void initUi();
    void initVariables();

public:
    explicit LoginState(std::shared_ptr<GameData> t_data);

    void init() override;
    void destroy() override;

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;
};
#endif
