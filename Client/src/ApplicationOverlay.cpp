#include "ApplicationOverlay.hpp"
#include "Definitions.hpp"

ApplicationOverlay::ApplicationOverlay(std::shared_ptr<tgui::Gui> t_gui):
    gui(t_gui){};

void ApplicationOverlay::init(){
    int current_width = gui->getWindow()->getSize().x;
    main_panel = tgui::ScrollablePanel::create({
            std::min(MAX_APPLICATION_OVERLAY_WIDTH, current_width),
            "70%"
            });
    main_panel->getRenderer()->setBackgroundColor(tgui::Color::Cyan);
    main_panel->setOrigin(0.5, 0);
    main_panel->setPosition("50%", "20%");

    background_panel = tgui::Panel::create({"100%", "100%"});
    background_panel->getRenderer()->setBackgroundColor(tgui::Color::Blue);
    background_panel->getRenderer()->setOpacity(0.8);


    gui->add(background_panel);
    gui->add(main_panel);
}

void ApplicationOverlay::destroy(){
    gui->remove(background_panel);
    gui->remove(main_panel);
}


void ApplicationOverlay::setEnabled(bool t_enabled){
    enabled = t_enabled;
    main_panel->setVisible(enabled);
    background_panel->setVisible(enabled);
    background_panel->moveToFront();
    main_panel->moveToFront();

}

void ApplicationOverlay::toggle(){
    enabled = !enabled;
    setEnabled(enabled);
}

void ApplicationOverlay::addButton(int slot, std::string identfier, std::string text, std::function<void()> func){
    if(slot >= MAX_APPLICATION_OVERLAY_SLOTS) return;
    buttons.resize(slot+1);
    int offset_y = APPLICATION_OVERLAY_BUTTON_HEIGHT * slot + APPLICATION_OVERLAY_BUTTON_SPACE * slot;
    int main_panel_width = main_panel->getSize().x;

    tgui::Button::Ptr button = tgui::Button::create(text);
    button->setOrigin(0.5f, 0.f);
    button->setSize(std::min(APPLICATION_OVERLAY_BUTTON_WIDTH, main_panel_width) , APPLICATION_OVERLAY_BUTTON_HEIGHT);
    button->setPosition("50%", offset_y);
    button->onPress(func);

    main_panel->add(button);

    buttons[slot] = std::make_pair(identfier, button);
}

bool ApplicationOverlay::removeButton(int slot){
    if(0 <= slot && (std::vector<std::pair<std::string, tgui::Button::Ptr>>::size_type) slot >= buttons.size()) return false;
    main_panel->remove(buttons[slot].second);
    return true;
}

bool ApplicationOverlay::removeButton(std::string identfier){
    for(std::vector<std::pair<std::string, tgui::Button::Ptr>>::size_type i = 0; i < buttons.size(); i++){
        if(buttons[i].first != identfier) continue;

        removeButton(i);
        return true;
    }
    return false;
}
