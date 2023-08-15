#ifndef DEFAULT_CONFIG_HPP
#define DEFAULT_CONFIG_HPP

#include <unordered_map>
#include <Setting.hpp>
#include <string>


std::unordered_map<std::string, Setting> DEFAULT_CONFIG{
    // Graphics
    {"WIDTH", Setting(SettingTag::GRAPHICS, 1920, 600, 3840*3)},
    {"HEIGHT", Setting(SettingTag::GRAPHICS, 1080, 200, 2160*3)},
    {"VSYNC", Setting(SettingTag::GRAPHICS, false, false, true)},
    {"FRAME_LIMIT", Setting(SettingTag::GRAPHICS, 144, 30, 480)},
    // Gameplay
    {"FORCED_FALLING_SPEED", Setting(SettingTag::GRAPHICS, 40, 1, 100)}, // Tetramino falls x times per 10 seconds
};

#endif
