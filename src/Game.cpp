#include "Game.hpp"

#include "GameState.hpp"

Game::Game(){
    srand(time(NULL));

    data->window->create(sf::VideoMode(WIDTH, HEIGHT), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar);
    data->gui.setWindow(*data->window);

    data->state_manager.switchToState(std::make_shared<GameState>(data));

    run();
};

void Game::run(){
    sf::Time new_time, frame_time, interpolation;

    sf::Time current_time = clock.getElapsedTime();
    sf::Time accumulator = sf::seconds(0.0f);
    
    /* Old fancy version
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
        data->state_manager.handleInputs();
        data->state_manager.updateState(current_time);
        data->state_manager.drawState();
    }
};
