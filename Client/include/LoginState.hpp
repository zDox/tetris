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
    std::shared_ptr<ApplicationData> data;

    // TGui elements
    tgui::Panel::Ptr panel;
    tgui::Label::Ptr heading;
    tgui::Label::Ptr label_ipaddress;
    tgui::EditBox::Ptr box_ipaddress;
    tgui::Label::Ptr label_username;
    tgui::EditBox::Ptr box_username;

    tgui::Button::Ptr button_connect;

    void login();
    void handleLoginResponseMessage(yojimbo::Message* t_message);

    void initWindow();
    void initUi();
    void initVariables();
    void initHandlers();
public:
    explicit LoginState(std::shared_ptr<ApplicationData> t_data);

    void init() override;
    void destroy() override;

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;
};
#endif
