#ifndef SETTINGS_OVERLAY_HPP
#define SETTINGS_OVERLAY_HPP

#include <memory>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "Config.hpp"
#include "Overlay.hpp"


// Its main porpose is to enable States to add buttons 
class SettingsOverlay : Overlay{
private:
    std::shared_ptr<tgui::Gui> gui;
    std::shared_ptr<Config> config;
    tgui::ScrollablePanel::Ptr main_panel;
    tgui::Panel::Ptr background_panel;
    bool enabled = false;

    // Tgui elements
    tgui::ComboBox::Ptr width_combobox;
    tgui::ComboBox::Ptr height_combobox;
    tgui::ToggleButton::Ptr vsync_togglebutton;
    tgui::Slider::Ptr frame_limit_slider;
    tgui::Slider::Ptr forced_falling_speed_slider;
public:
    SettingsOverlay(std::shared_ptr<tgui::Gui> t_gui, std::shared_ptr<Config> t_config);
    void init() override;
    void destroy() override;

    void applySettings();
};

#endif 
