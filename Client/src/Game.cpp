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
    sf::Time next_cycle, frame_time, last_tick=sf::seconds(0.f);
    sf::Time fixed_dt = sf::seconds(1.0f / (float)TICK_RATE);

    clock.restart();
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
        data->state_manager.drawState();
    } 
    */
    while (data->window->isOpen()){

        if(next_cycle <= clock.getElapsedTime()){
            CORE_TRACE("PERFORMANCE - Last tick: {}us next_cycle: {}", ((clock.getElapsedTime() - last_tick).asMicroseconds()), next_cycle.asMicroseconds());
            data->state_manager.processStateChanges();
            data->state_manager.handleInputs();
            data->state_manager.updateState(frame_time);
            data->network_manager.update();
            data->state_manager.drawState();
            next_cycle += fixed_dt;
            last_tick = clock.getElapsedTime();
        }
        else {
            sf::sleep(next_cycle - clock.getElapsedTime());
        }
    }  
};

void Game::close(){
    data->network_manager.destroy();
}
