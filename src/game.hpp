#ifndef GAME_HPP
#define GAME_HPP

#include "tetramino.hpp"
#include <memory>
#include <vector>

// 3rd Party libraries
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


class Game{
private:
    static const int FPS = 60;
    static const int HEIGHT = 800;
    static const int WIDTH = 600;
    static const int ROWS = 20;
    static const int COLUMNS = 10;

    static const int SPACING_TOP = 10; // Spacing between Window and upper grid 
    static const int SPACING_BOTTOM = 10; // Spacing between Window and bottom grid     
    static const int SPACING_LEFT = 10; // Spacing between Window and left grid

    static const int SPACING_PER_RECT = 4.f; // Spacing between every rect in grid

    static const float SIDE_LENGTH; // Gets calculated later (Not possible here)

    // SFML
    std::shared_ptr<sf::RenderWindow> window;
    sf::VideoMode videomode;
    sf::Event event;

    // Game Logic
    // Game Logic -- Config
    static const int SPEED = 4; // Move tetra 4 Grid slots per second
                            
    // Game Logic -- Variables
    bool running = true;
    std::vector<std::vector<sf::RectangleShape>> grid;
    std::shared_ptr<Tetramino> tetra;

    // Game Logic -- Timer
    sf::Clock tetra_move_timer;

    // Game Logic -- Functions
    void spawnTetramino();

    // Drawing
    void drawGrid();
    
    // Updating
    void resetGrid();
    void putTetraOnGrid();
    void updateTetra();

    void initWindow();
    void initVariables();
    
    void handleEvents();
    void update();
    void render();

public:
    Game();

    void run();
    void close();

    bool isRunning();
};

#endif
