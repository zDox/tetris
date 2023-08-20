#include "DefaultConfig.hpp"

std::unordered_map<std::string, std::variant<Setting<bool>, Setting<int>, Setting<double>>> DEFAULT_CONFIG = {
    // Graphics
    {"WIDTH", Setting<int>(SettingTag::GRAPHICS, 
              1920, 
              std::make_shared<Setting<int>::DiscreteValueValidator>(std::vector{1920}))},
    {"HEIGHT", Setting<int>(SettingTag::GRAPHICS, 
              1080, 
              std::make_shared<Setting<int>::DiscreteValueValidator>(std::vector{1080}))},
    {"VSYNC", Setting<bool>(SettingTag::GRAPHICS, 
              false, 
              std::make_shared<Setting<bool>::DiscreteValueValidator>(std::vector{false, true}))}, 
    {"FRAME_LIMIT", Setting<int>(SettingTag::GRAPHICS, 
              0, 
              std::make_shared<Setting<int>::RangeValueValidator>(std::vector{Setting<int>::RangeValueValidator::Range(0, 0), Setting<int>::RangeValueValidator::Range(30, 460)}))}, 
    // Gameplay

    // Tetramino falls an additional x times per 10 seconds
    {"FORCED_FALLING_SPEED", Setting<double>(SettingTag::GRAPHICS, 
              40.0, 
              std::make_shared<Setting<double>::RangeValueValidator>(std::vector{Setting<double>::RangeValueValidator::Range(1.0, 100.0)}))}, 
};
