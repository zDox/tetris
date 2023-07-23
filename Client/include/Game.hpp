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
#include "StateManager.hpp"
#include "NetworkManager.hpp"

enum CollisionType{
    NONE,
    SIDE,
    BOTTOM
};

struct GameData{
    StateManager state_manager;
    NetworkManager network_manager;
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>();
    tgui::Gui gui;
};

class Game{
    // Updates run at 60 per second.
    sf::Time dt = sf::seconds(1.0f / 60.0f);
    sf::Clock clock;

    std::shared_ptr<GameData> data = std::make_shared<GameData>();
public:
    Game();

    bool init();
    void run();

    void close();
};

#endif
