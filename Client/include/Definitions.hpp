#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

// Window related
#define WINDOW_TITLE "Tetris"
#define MENU_FPS 60
#define GAME_FPS 144
#define HEIGHT 800
#define WIDTH 1000

#define BACKGROUND_COLOR sf::Color::Black
#define GRID_COLOR sf::Color(0, 128, 255)

#define SPACING_TOP 10 // Spacing between Window and upper grid 
#define SPACING_BOTTOM 10 // Spacing between Window and bottom grid     
#define SPACING_LEFT 10 // Spacing between Window and left grid

#define SPACING_PER_RECT 4.0 // Spacing between every rect in grid
#define SPACING_BETWEEN_GRIDS 10 // Spaing between each opponent grid or the players own grid

#define SIDE_LENGTH static_cast<float>((HEIGHT-(SPACING_TOP + SPACING_BOTTOM)) / static_cast<float>(ROWS) - SPACING_PER_RECT)

// Gameplay
#define ROWS 20
#define COLUMNS 10
#define SPEED 4 // 4 Grid pixel per 10 Seconds
#endif