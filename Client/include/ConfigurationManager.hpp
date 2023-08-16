#ifndef CONFIGURATION_MANAGER_HPP
#define CONFIGURATION_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>

#include "Log.hpp"
#include "Setting.hpp"
#include "DefaultConfig.hpp"


class ConfigurationManager{
private:
    std::unordered_map<std::string, Setting> settings;
    std::function<void()> graphics_handler;

    void resetSetting(std::string key);

    template <typename T>
    void setValue(std::string key, T value){
        if(DEFAULT_CONFIG.contains(key)){
            if(!settings.contains(key)) {
                CORE_WARN("ConfigurationManager - setValue - Setting: {} is not set. Fallback to default value.", key);
                settings.emplace(std::make_pair(key, DEFAULT_CONFIG[key]));
                return;
            }
            settings[key].setValue(value);
        }
        else{
            throw std::runtime_error("ConfigurationManager - setValue - Setting '" + key + "' is not valid.");
        }
    }

    template <typename T>
    T getValue(std::string key){
        if(!settings.contains(key)){
            if(DEFAULT_CONFIG.contains(key)){
                CORE_WARN("ConfigurationManager - getValue - Setting: {} is not set. Fallback to default value.", key);
                settings.emplace(key, DEFAULT_CONFIG[key]);
                return std::get<T>(DEFAULT_CONFIG[key].getValue());
            }
            throw std::runtime_error("ConfigurationManager - getValue - Setting '" + key + "' is not valid"); 
        }
        else {
            return std::get<T>(settings[key].getValue());
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
