#ifndef APPLICATION_OVERLAY_HPP
#define APPLICATION_OVERLAY_HPP


#include <memory>
#include <string>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "Overlay.hpp"

struct ButtonSlot{
    int slot;
    tgui::Button::Ptr ptr;
    std::string identifier;
    std::function<void()> func;
};

// Its main porpose is to enable States to add buttons 
class ApplicationOverlay : public Overlay{
private:
    std::vector<ButtonSlot> buttons;
    std::shared_ptr<tgui::Gui> gui;
    tgui::ScrollablePanel::Ptr main_panel;
    tgui::Panel::Ptr background_panel;
    bool enabled = false;
public:
    ApplicationOverlay(std::shared_ptr<tgui::Gui> t_gui);
    void init() override;
    void destroy() override;

    void pressButton(std::string identifier);
    void addButton(int slot, std::string identifier, std::string text, std::function<void()> func);
    bool removeButton(int slot);
    bool removeButton(std::string identifier);
};

#endif 
