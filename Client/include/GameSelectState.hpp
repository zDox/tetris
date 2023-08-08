#ifndef GAMESELECTSTATE_HPP
#define GAMESELECTSTATE_HPP


#include "GameState.hpp"
#include "State.hpp"
#include "GameDefinitions.hpp"


class GameSelectState : public State{
private:
    std::shared_ptr<GameData> data;

    // TGui elements

    void initWindow();
    void initUi();
    void initVariables();
    void initHandlers();
public:
    explicit GameSelectState(std::shared_ptr<GameData> t_data);

    void init() override;
    void destroy() override;

    void handleInputs() override;
    void update(sf::Time dt) override;
    void draw() override;
};
#endif
