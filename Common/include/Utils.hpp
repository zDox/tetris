#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include<algorithm>
#include <cinttypes>

#include <SFML/Graphics/Color.hpp>

void transpose(std::vector<std::vector<bool>> &matrix);
void reverse_rows(std::vector<std::vector<bool>> &matrix);
bool vecsAreEqual(std::vector<std::vector<sf::Color>> vec1, std::vector<std::vector<sf::Color>> vec2);
std::vector<std::vector<uint32_t>> convertGridToColors(std::vector<std::vector<sf::Color>> grid);
#endif
