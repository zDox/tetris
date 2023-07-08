#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <vector>

// 3rd Party libraries
#include <SFML/Graphics.hpp>

class Game{
private:
    // SFML
    std::shared_ptr<sf::RenderWindow> window;
    sf::VideoMode videomode;
    sf::Event event;

    // Game Logic
    bool running = true;

    void initWindow();

    void update();
    void render();

public:
    Game();

    void run();
    void close();

    bool isRunning();
};

#endif
