#include "LoginState.hpp"

LoginState::LoginState(std::shared_ptr<GameData> t_data):data(t_data){};

void LoginState::initWindow(){
    data->window->setFramerateLimit(MENU_FPS);
}

void LoginState::initVariables(){
}

void LoginState::initUi(){
    panel = tgui::Panel::create();
    panel->setOrigin(0.5, 0);
    panel->setPosition(WIDTH/2, 0);
    panel->setSize(WIDTH/2 < 600 ? WIDTH : WIDTH/2, HEIGHT); 
    data->gui.add(panel);

    heading = tgui::Label::create("Connect to Server");
    heading->setOrigin(0.5f, 0);
    heading->setPosition(WIDTH/2, "1%");
    heading->setTextSize(40);
    panel->add(heading);

    label_ipaddress = tgui::Label::create("Server Address:");
    label_ipaddress->setPosition(WIDTH/4, "50%");
    label_ipaddress->setTextSize(20);
    panel->add(label_ipaddress);
   
    box_ipaddress = tgui::EditBox::create();
    box_ipaddress->setDefaultText("127.0.0.1");
    box_ipaddress->setText("127.0.0.1");
    box_ipaddress->setPosition(tgui::bindRight(label_ipaddress) + 10.f, "50%");
    box_ipaddress->setTextSize(20);
    panel->add(box_ipaddress);

    button_connect = tgui::Button::create("CONNECT");
    button_connect->setTextSize(30);
    button_connect->setOrigin(0.5f, 0);
    button_connect->setPosition(WIDTH/2, "80%");
    button_connect->onPress(&LoginState::login, this);
    panel->add(button_connect);
}

void LoginState::init(){
    initWindow();
    initVariables();
    initUi();
}

void LoginState::destroy(){
    data->gui.removeAllWidgets();
}

void LoginState::login(){
    std::cout << "Address: " << box_ipaddress->getText() << "\n";
    data->network_manager.connect(box_ipaddress->getText().toStdString());
};

void LoginState::handleInputs(){
    sf::Event event;
    while(data->window->pollEvent(event)){
        data->gui.handleEvent(event);
        if(event.type == sf::Event::Closed){
            data->window->close(); 
        }
    }
}

void LoginState::update(sf::Time dt){
    if(data->network_manager.getConnectionStatus() == ConnectionStatus::CONNECTED){
        data->state_manager.switchToState(std::make_shared<GameState>(data));
    }
}

void LoginState::draw(){
    data->window->clear(BACKGROUND_COLOR);

    data->gui.draw();

    data->window->display();
}
