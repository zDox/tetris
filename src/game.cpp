#include "game.hpp"

Game::Game(){
    initWindow();
}

void Game::initWindow(){
    videomode.height = 800;
    videomode.width = 600;

    window = std::make_shared<sf::RenderWindow>(videomode, "Snake Game", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);
}

void Game::update(){
    while(window->pollEvent(event)){
        if(event.type == sf::Event::Closed){
            running = false;
        }
    }
};

void Game::render(){
    window->clear();

    window->display();
};

void Game::run(){
    update();
    render();
}

void Game::close(){
    window->close();
}

bool Game::isRunning(){
    return running;
}
