#ifndef CONFIGURATION_MANAGER_HPP
#define CONFIGURATION_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
#include <functional>
#include <variant>

#include "Log.hpp"

enum class SettingTag{
    GRAPHICS,
    GAMEPLAY,
};

class Setting{
public:
    enum class Type {Bool, Int, Double};

    template <typename T>
    Setting(SettingTag t_tag, T t_value, T t_min_value, T t_max_value) : 
        tag(t_tag),
        value(t_value), 
        min_value(t_min_value), 
        max_value(t_max_value)
        {};
    SettingTag getTag(){
        return tag;
    }

    Type getType() const {
        return static_cast<Type>(value.index());
    }

    template <typename T>
    T getValue() const {
        return std::get<T>(value);
    }

    template <typename T> 
    void setValue(T t_value){
        if(t_value >= std::get<T>(min_value) && t_value <= std::get<T>(max_value)){
            value = t_value;
        }
        else {
            CORE_WARN("ConfigurationManager - out_of_range value: {}, min: {}, max:{}", t_value, std::get<T>(min_value), std::get<T>(max_value));
            value = std::max(std::get<T>(min_value), std::min(t_value, std::get<T>(max_value)));
        }
    }

    
private:
    SettingTag tag;
    std::variant<bool, std::int64_t, double> value;
    std::variant<bool, std::int64_t, double> min_value;
    std::variant<bool, std::int64_t, double> max_value;
};

std::unordered_map<std::string, Setting> DEFAULT_CONFIG{
    // Graphics
    {"WIDTH", Setting(SettingTag::GRAPHICS, 1920, 600, 3840*3)},
    {"HEIGHT", Setting(SettingTag::GRAPHICS, 1080, 200, 2160*3)},
    {"VSYNC", Setting(SettingTag::GRAPHICS, false, false, true)},
    {"FRAME_LIMIT", Setting(SettingTag::GRAPHICS, 144, 30, 480)},
    // Gameplay
    {"FORCED_FALLING_SPEED", Setting(SettingTag::GRAPHICS, 40, 1, 100)}, // Tetramino falls x times per 10 seconds
};

class ConfigurationManager{
private:
    std::unordered_map<std::string, Setting> settings;
    std::function<void()> graphics_handler;

    void resetSetting(std::string key);

    template <typename T>
    void setSetting(std::string key, T value){
        if(!DEFAULT_CONFIG.contains(key)){
            CORE_WARN("ConfigurationManager - setSetting - Setting '{}' is not a default setting", key);
            settings.emplace(key, value);
        }
        else{
            settings[key].setValue(value);
        }
    }

    template <typename T>
    T getSetting(std::string key){
        if(!settings.contains(key)){
            if(DEFAULT_CONFIG.contains(key)){
                CORE_WARN("ConfigurationManager - Setting: {} is not set. Fallback to default value: {}", key, DEFAULT_CONFIG[key].getValue<T>());
                setInt(key, DEFAULT_CONFIG[key].getValue<T>());
                return DEFAULT_CONFIG[key].getValue<T>();
            }
            else {
                throw std::runtime_error("ConfigurationManager - getSetting - Setting '" + key + "' is not valid");
            }
        }
        else {
            return settings[key].getValue<T>();
        }
    }

public:
    void setBool(std::string key, bool value);
    void setInt(std::string key, int value);
    void setDouble(std::string key, double value);

    bool getBool(std::string key);
    int getInt(std::string key);
    double getDouble(std::string key);


    void registerGraphicsHandler(std::function<void()> func);

    void loadSettings();
    void saveSettings();
};

#endif
