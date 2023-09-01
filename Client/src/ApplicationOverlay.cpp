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

void ApplicationOverlay::pressButton(std::string identifier){
    setEnabled(false);
    for(const ButtonSlot& button_slot : buttons){
        if(button_slot.identifier != identifier) continue;
        button_slot.func();
    }
}

void ApplicationOverlay::addButton(int slot, std::string identfier, std::string text, std::function<void()> func){
    if(slot >= MAX_APPLICATION_OVERLAY_SLOTS) return;
    
    // Check if slot is taken
    int remove_slot = -1;
    for(const ButtonSlot& button_slot : buttons){
        if(button_slot.slot == slot){
            remove_slot = slot;
        }
    }
    if(remove_slot != -1) removeButton(slot);
        

    int offset_y = APPLICATION_OVERLAY_BUTTON_HEIGHT * slot + APPLICATION_OVERLAY_BUTTON_SPACE * slot;
    int main_panel_width = main_panel->getSize().x;

    ButtonSlot button_slot;
    tgui::Button::Ptr button = tgui::Button::create(text);
    button->setUserData(identfier);
    button->setOrigin(0.5f, 0.f);
    button->setSize(std::min(APPLICATION_OVERLAY_BUTTON_WIDTH, main_panel_width) , APPLICATION_OVERLAY_BUTTON_HEIGHT);
    button->setPosition("50%", offset_y);
    button->onPress(&ApplicationOverlay::pressButton, this, identfier);

    main_panel->add(button);
    
    button_slot.slot = slot;
    button_slot.ptr = button;
    button_slot.identifier = identfier;
    button_slot.func = func;

    buttons.push_back(button_slot);
}

bool ApplicationOverlay::removeButton(int slot){
    if(0 <= slot && (std::vector<std::pair<std::string, tgui::Button::Ptr>>::size_type) slot >= buttons.size()) return false;
    main_panel->remove(buttons[slot].ptr);
    buttons.erase(std::remove_if(buttons.begin(), buttons.end(), [slot](const ButtonSlot& button) {
        return button.slot == slot;
    }), buttons.end());    
    return true;
}

bool ApplicationOverlay::removeButton(std::string identfier){
    for(std::vector<std::pair<std::string, tgui::Button::Ptr>>::size_type i = 0; i < buttons.size(); i++){
        if(buttons[i].identifier != identfier) continue;

        removeButton(i);
        return true;
    }
    return false;
}
