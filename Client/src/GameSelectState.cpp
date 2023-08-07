#include "GameSelectState.hpp"

GameSelectState::GameSelectState(std::shared_ptr<GameData> t_data):data(t_data){};

void GameSelectState::initWindow(){
    data->window->setFramerateLimit(MENU_FPS);
}

void GameSelectState::initVariables(){
}

void GameSelectState::initUi(){
}

void GameSelectState::init(){
    initWindow();
    initVariables();
    initUi();
}

void GameSelectState::destroy(){
    data->gui.removeAllWidgets();
}

void GameSelectState::handleInputs(){
    sf::Event event;
    while(data->window->pollEvent(event)){
        data->gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close(); 
        }
    }
}

void GameSelectState::update(sf::Time dt){
}

void GameSelectState::draw(){
    data->window->clear(BACKGROUND_COLOR);

    data->gui.draw();

    data->window->display();
}
