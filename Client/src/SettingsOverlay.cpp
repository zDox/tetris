#include "SettingsOverlay.hpp"
#include "Definitions.hpp"

SettingsOverlay::SettingsOverlay(std::shared_ptr<tgui::Gui> t_gui):
    gui(t_gui){};

void SettingsOverlay::init(){
    int current_width = gui->getWindow()->getSize().x;
    main_panel = tgui::ScrollablePanel::create({
            std::min(MAX_APPLICATION_OVERLAY_WIDTH, current_width),
            "100%"
            });
    main_panel->getRenderer()->setBackgroundColor(tgui::Color::Cyan);
    main_panel->setOrigin(0.5, 0);
    main_panel->setPosition("50%", "0%");

    background_panel = tgui::Panel::create({"100%", "100%"});
    background_panel->getRenderer()->setBackgroundColor(tgui::Color::Blue);
    background_panel->getRenderer()->setOpacity(0.8);

    gui->add(background_panel);
    gui->add(main_panel);

}

void SettingsOverlay::destroy(){
    gui->remove(background_panel);
    gui->remove(main_panel);
}
