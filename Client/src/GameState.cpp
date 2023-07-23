#include "GameState.hpp"
#include "Definitions.hpp"

GameState::GameState(std::shared_ptr<GameData> t_data) : data(t_data) {
}

void GameState::init(){
    initWindow();
    initUI();
}

void GameState::destroy(){
    data->gui.removeAllWidgets();
};


void GameState::initWindow(){  
    data->window->setFramerateLimit(GAME_FPS);
}

void GameState::initUI(){
    points_label = tgui::Label::create("None");
    points_label->setPosition(400, 10);
    points_label->setTextSize(18);
    data->gui.add(points_label);

    paused_label = tgui::Label::create("Paused");
    paused_label->setOrigin(0.5f, 0.5f); // Set its anker to center of label 
    paused_label->setPosition(WIDTH/2.f, HEIGHT/2.f);
    paused_label->setTextSize(30);

    data->gui.add(paused_label);
}


 

void GameState::updateUI(){
    points_label->setText("Points: " + std::to_string(points));
}

void GameState::drawGrid(){
    for(int i=0; i < ROWS; i++){
        for(int k=0; k<COLUMNS; k++){
            grid[i][k].setPosition(k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_LEFT, i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_TOP);
            data->window->draw(grid[i][k]);
        }
    }
}

void GameState::drawOpponentGrids(){
    std::unordered_map<uint64_t, std::vector<std::vector<uint32_t>>> oponent_grids = data->network_manager.getOpponentsGrid();
    int grid_pixel_width = ((SPACING_PER_RECT+SIDE_LENGTH)*COLUMNS);
    int max_grids_to_draw = std::min((int) oponent_grids.size()+1, (int)std::floor(WIDTH/grid_pixel_width));
    int count = 1; // Count of how many grids have been drawn
    for(auto[client_id, opponent_grid] : oponent_grids){
        if(count >= max_grids_to_draw) break;
        for(int i = 0; i < ROWS; i++){
            for(int k = 0; k < COLUMNS; k++){
                sf::RectangleShape rect(sf::Vector2(SIDE_LENGTH, SIDE_LENGTH));
                int pos_x = k*SIDE_LENGTH + k*SPACING_PER_RECT + SPACING_LEFT + count*grid_pixel_width + count*SPACING_BETWEEN_GRIDS;
                int pos_y = i*SIDE_LENGTH + i*SPACING_PER_RECT + SPACING_TOP;
                uint32_t color_uint = opponent_grid[i][k];
                uint8_t red = (color_uint >> 24) & 0xFF;
                uint8_t green = (color_uint >> 16) & 0xFF;
                uint8_t blue = (color_uint >> 8) & 0xFF;
                uint8_t alpha = color_uint & 0xFF;
                sf::Color color(red, green, blue, alpha);
                rect.setPosition(pos_x, pos_y);
                rect.setFillColor(color);
                data->window->draw(rect);
            }
        }
        count+=1;
    }
}

void GameState::handleInputs(){
    sf::Event event;
    while(data->window->pollEvent(event)){
        data->gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close();
        }
    }
};


void GameState::update(sf::Time dt){ 
    game_logic.update(dt);
    updateUI();
};


void GameState::draw(){
    data->window->clear(BACKGROUND_COLOR);
    
    drawGrid();
    drawOpponentGrids();

    data->gui.draw();
     
    data->window->display();
};
