#include "Game.hpp"

#include "LoginState.hpp"

Game::Game(){
    srand(time(NULL));
};

bool Game::init(){
    
    if(!data->network_manager.init()) return false;

    sf::VideoMode desktop_mode = sf::VideoMode::getDesktopMode();
    data->window->create(sf::VideoMode(WIDTH, HEIGHT, desktop_mode.bitsPerPixel), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar);
    data->gui.setWindow(*data->window);


    data->state_manager.switchToState(std::make_shared<LoginState>(data));
    return true;
}
void Game::run(){
    sf::Time new_time, frame_time, interpolation;

    sf::Time current_time = clock.getElapsedTime();
    /*
    sf::Time accumulator = sf::seconds(0.0f);
    
    while(data->window->isOpen()){
        data->state_manager.processStateChanges();

        new_time = clock.getElapsedTime();

        frame_time = std::max(new_time - current_time, sf::seconds(0.25f));

        current_time = new_time;
        accumulator += frame_time;

        while(accumulator >= dt){
            data->state_manager.handleInputs();
            data->state_manager.updateState(dt);

            accumulator -= dt;
        }
        interpolation = sf::seconds(accumulator / dt);
        data->state_manager.drawState();
    } 
    */ 
    
    while (data->window->isOpen()){
        data->state_manager.processStateChanges();
        data->network_manager.update();
        data->state_manager.handleInputs();
        data->state_manager.updateState(current_time);
        data->state_manager.drawState();
    } 
};

void Game::close(){
    data->network_manager.destroy();
}
