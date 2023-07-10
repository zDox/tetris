#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <vector>
#include <string>

// 3rd Party libraries
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "tetramino.hpp"
#include "utils.hpp"

enum COLLISION_TYPE{
    NONE,
    SIDE,
    BOTTOM
};

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

    sf::Color BACKGROUND_COLOR = sf::Color::Black;
    sf::Color GRID_COLOR = sf::Color(0, 128, 255);

    // SFML
    std::shared_ptr<sf::RenderWindow> window;
    sf::VideoMode videomode;
    sf::Event event;

    // SFML -- initUI
    std::shared_ptr<sf::Font> roboto_font;
    std::shared_ptr<sf::Text> points_text;
    std::shared_ptr<sf::Text> paused_text;

    // Game Logic
    // Game Logic -- Config
    static const int SPEED = 20; // Move tetra SPEED Grid slots per 10 second
                            
    // Game Logic -- Variables
    bool running = true;
    bool paused = false;
    bool finished = false;
    std::vector<std::vector<sf::RectangleShape>> grid;
    std::shared_ptr<Tetramino> tetra;
    int points = 0;

    std::vector<std::vector<sf::RectangleShape>> stationaries;
    bool hold_up = false;
    bool hold_left = false;
    bool hold_right = false;

    // Game Logic -- Timer
    sf::Clock tetra_move_timer;

    // Game Logic -- Functions
    void spawnTetramino();
    bool checkCollisions(int dx, int dy, bool clockwise);
    void lockTetra();

    bool checkLoss();
    void handleLoss();

    // Updating
    void resetGrid();
    void putTetraOnGrid();
    void putStationariesOnGrid();
    void updateTetra();
    void checkPoint();
    void updateUI();

    // Drawing
    void drawGrid();
    void drawUI();
    
    void initWindow();
    void initVariables();
    void initUI();
    
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
