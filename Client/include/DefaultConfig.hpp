#ifndef DEFAULT_CONFIG_HPP
#define DEFAULT_CONFIG_HPP

#include <unordered_map>
#include <Setting.hpp>
#include <string>


extern std::unordered_map<std::string, std::variant<Setting<bool>, Setting<int>, Setting<double>>> DEFAULT_CONFIG;

#endif
