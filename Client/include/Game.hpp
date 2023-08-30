#ifndef GAME_HPP
#define GAME_HPP

#include <memory>

// 3rd Party libraries
// SFML
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
// tgui
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "Definitions.hpp"
#include "ApplicationOverlay.hpp"
#include "StateManager.hpp"
#include "NetworkManager.hpp"
#include "Config.hpp"


struct ApplicationData{
    StateManager state_manager;
    NetworkManager network_manager;
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>();
    std::shared_ptr<Config> config;
    std::shared_ptr<tgui::Gui> gui;
    std::shared_ptr<ApplicationOverlay> overlay;
    int game_id = -1;
};

class Game{
    // Updates run at 60 per second.
    sf::Time dt = sf::seconds(1.0f / 60.0f);
    sf::Clock clock;

    std::shared_ptr<ApplicationData> data = std::make_shared<ApplicationData>();
public:
    Game();

    bool init();
    void run();

    void close();
};

#endif
