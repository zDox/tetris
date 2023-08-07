#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cinttypes>
#include <vector>
#include <string>

struct Player{
    uint64_t client_id;
    std::string name;
    std::vector<std::vector<uint32_t>> grid;
    int points = 0;
    int position = -1;
};

struct PlayerInput{
    bool left = false, right = false, up=false, down=false;
    long frame;
};

enum class LoginResult{
    NONE,
    SUCCESS,
    TAKEN_NAME,
    INVALID_NAME,
};

bool comp(const PlayerInput &l, const long &frame);

bool compPlayerByPoints(const Player &l, const Player &r);

#endif
