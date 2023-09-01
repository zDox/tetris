#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include <memory>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

class Overlay{
    std::shared_ptr<tgui::Gui> gui;
    tgui::ScrollablePanel::Ptr main_panel;
    tgui::Panel::Ptr background_panel;
    bool enabled = false;
public:

    virtual void init() = 0;
    virtual void destroy() = 0;

    void setEnabled(bool t_enabled);
    void toggle();
};

#endif 
