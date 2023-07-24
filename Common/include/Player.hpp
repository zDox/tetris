#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cinttypes>
#include <vector>

struct Player{
    uint64_t client_id;
    std::vector<std::vector<uint32_t>> grid;
    int points = 0;
    int position = -1;
};
#endif
