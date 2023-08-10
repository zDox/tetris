#include "GameSelectState.hpp"
#include "LoginState.hpp"

LoginState::LoginState(std::shared_ptr<ApplicationData> t_data):data(t_data){};

void LoginState::initWindow(){
    data->window->setFramerateLimit(MENU_FPS);
}

void LoginState::initVariables(){
    data->network_manager.start();
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
    box_ipaddress->setPosition(tgui::bindRight(label_ipaddress) + 10.f, tgui::bindTop(label_ipaddress));
    box_ipaddress->setTextSize(20);
    panel->add(box_ipaddress);

    label_username = tgui::Label::create("Username:");
    label_username->setPosition(WIDTH/4, tgui::bindBottom(label_ipaddress) + 10.f);
    label_username->setTextSize(20);
    panel->add(label_username);
   
    box_username = tgui::EditBox::create();
    box_username->setDefaultText("Mustermann");
    box_username->setText("Mustermann");
    box_username->setPosition(tgui::bindRight(label_username) + 10.f, tgui::bindTop(label_username));
    box_username->setTextSize(20);
    panel->add(box_username);


    button_connect = tgui::Button::create("CONNECT");
    button_connect->setTextSize(30);
    button_connect->setOrigin(0.5f, 0);
    button_connect->setPosition(WIDTH/2, "80%");
    button_connect->onPress(&LoginState::login, this);
    panel->add(button_connect);
}

void LoginState::initHandlers(){
    data->network_manager.registerMessageHandler(
            MessageType::LOGIN_RESPONSE, 
            std::bind(&LoginState::handleLoginResponseMessage, this, std::placeholders::_1));
}

void LoginState::init(){
    initWindow();
    initVariables();
    initHandlers();
    initUi();
}

void LoginState::destroy(){
    data->gui.removeAllWidgets();
    data->network_manager.unregisterMessageHandlers();
}

void LoginState::login(){
    data->network_manager.connect(box_ipaddress->getText().toStdString());
    std::string username = box_username->getText().toStdString();
    if(!(0 < username.size() && username.size() <=128)) return;

    data->network_manager.queueLoginRequest(username);
};

void LoginState::handleLoginResponseMessage(yojimbo::Message* t_message){
    LoginResponseMessage* message = (LoginResponseMessage*) t_message;
    switch (message->result){
        case LoginResult::SUCCESS:
            CORE_INFO("LoginState - Authentifaction - Success logged username: {}", message->username);
            data->state_manager.switchToState(std::make_shared<GameSelectState>(data));
            data->network_manager.stop();
            break;
        case LoginResult::TAKEN_NAME:
            CORE_INFO("LoginState - Authentifaction - Username '{}' was already taken.", message->username);
            break;
        case LoginResult::INVALID_NAME:
            CORE_INFO("LoginState - Authentifaction - Username '{}' is invalid.", message->username);
            break;
        default:
            CORE_INFO("LoginState - Authentifaction - Result: {} is undefined", (int) message->result);
            break;
    }
}

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
    data->network_manager.update();
}

void LoginState::draw(){
    data->window->clear(BACKGROUND_COLOR);

    data->gui.draw();

    data->window->display();
}
