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
    int width = data->config.getInt("WIDTH");
    int height = data->config.getInt("HEIGHT");
    panel = tgui::Panel::create();
    panel->setOrigin(0, 0);
    panel->setPosition(0, 0);
    panel->setSize(width/2/2 < 600 ? width : width/2, height); 
    data->gui.add(panel);

    heading = tgui::Label::create("Connect to Server");
    heading->setOrigin(0.5f, 0);
    heading->setPosition("50%", "1%");
    heading->setTextSize(40);
    panel->add(heading);

    label_ipaddress = tgui::Label::create("Server Address:");
    label_ipaddress->setPosition("25%", "50%");
    label_ipaddress->setTextSize(20);
    panel->add(label_ipaddress);
   
    box_ipaddress = tgui::EditBox::create();
    box_ipaddress->setDefaultText("127.0.0.1");
    box_ipaddress->setText("127.0.0.1");
    box_ipaddress->setPosition(tgui::bindRight(label_ipaddress) + 10.f, tgui::bindTop(label_ipaddress));
    box_ipaddress->setTextSize(20);
    panel->add(box_ipaddress);

    label_username = tgui::Label::create("Username:");
    label_username->setPosition("25%", tgui::bindBottom(label_ipaddress) + 10.f);
    label_username->setTextSize(20);
    panel->add(label_username);
   
    box_username = tgui::EditBox::create();
    box_username->setText("Mustermann");
    box_username->setPosition(tgui::bindRight(label_username) + 10.f, tgui::bindTop(label_username));
    box_username->setTextSize(20);
    panel->add(box_username);

    status_text = tgui::Label::create();
    status_text->setOrigin(0.5f, 0);
    status_text->setPosition("50%", tgui::bindBottom(label_username) + 10.f);
    status_text->setTextSize(20);
    panel->add(status_text);

    button_connect = tgui::Button::create("CONNECT");
    button_connect->setTextSize(30);
    button_connect->setOrigin(0.5f, 0);
    button_connect->setPosition("50%", "80%");
    button_connect->onPress(&LoginState::login, this);
    panel->add(button_connect);
}

void LoginState::initHandlers(){
    data->network_manager.registerMessageHandler(
            MessageType::LOGIN_RESPONSE, 
            std::bind(&LoginState::handleLoginResponseMessage, this, std::placeholders::_1));
    data->network_manager.registerConnectionStatusHandler(
            std::bind(&LoginState::handleConnectionStatusChange, this, std::placeholders::_1));
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
    data->network_manager.unregisterConnectionStatusHandler();
}

void LoginState::login(){
    std::string username = box_username->getText().toStdString();
    if(!(0 < username.length() && username.length() <=128)) {
        status_text->getRenderer()->setTextColor(tgui::Color::Red);
        status_text->setText("Username '" + username +"' is invalid!");
        return;
    };
    data->network_manager.connect(box_ipaddress->getText().toStdString());

    data->network_manager.queueLoginRequest(username);
};

void LoginState::handleLoginResponseMessage(yojimbo::Message* t_message){
    LoginResponseMessage* message = (LoginResponseMessage*) t_message;
    status_text->getRenderer()->setTextColor(tgui::Color::Red);
    switch (message->result){
        case LoginResult::SUCCESS:
            CORE_INFO("LoginState - Authentifaction - Success logged username: {}", message->username);
            data->state_manager.switchToState(std::make_shared<GameSelectState>(data));
            data->network_manager.stop();
            break;
        case LoginResult::TAKEN_NAME:
            CORE_INFO("LoginState - Authentifaction - Username '{}' was already taken.", message->username);
            status_text->setText("Username " + message->username +" was already taken!");
            break;
        case LoginResult::INVALID_NAME:
            CORE_INFO("LoginState - Authentifaction - Username '{}' is invalid.", message->username);
            status_text->setText("Username " + message->username +" is invalid!");
            break;
        default:
            CORE_INFO("LoginState - Authentifaction - Result: {} is undefined", (int) message->result);
            status_text->setText("Unkown error occurred!");
            break;
    }
}

void LoginState::handleConnectionStatusChange(ConnectionStatus status){
    if(status == ConnectionStatus::ERROR_CONNECTION){
        status_text->setText("Server not found");
        status_text->getRenderer()->setTextColor(tgui::Color::Red);
    }
    else if(status == ConnectionStatus::CONNECTING){
        status_text->setText("Connecting to server ...");
        status_text->getRenderer()->setTextColor(tgui::Color::Green);
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
