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
    std::unordered_map<SettingTag, std::function<void()>> handlers;

    void resetSetting(std::string key);

    template <typename T>
    void setValue(std::string key, T value){
        if(settings.contains(key)) {
            settings[key].setValue(value);
            if(handlers.contains(settings[key].getTag())) {
                handlers[settings[key].getTag()]();
            }
        }
        else{
            throw std::runtime_error("ConfigurationManager - setValue - Setting '" + key + "' is not valid.");
        }
    }

    template <typename T>
    T getValue(std::string key){
        if(settings.contains(key)){
            return std::get<T>(settings[key].getValue());
        }
        else {
            throw std::runtime_error("ConfigurationManager - getValue - Setting '" + key + "' is not valid"); 
        }
    }

public:
    void setBool(std::string key, bool value);
    void setInt(std::string key, int value);
    void setDouble(std::string key, double value);

    bool getBool(std::string key);
    int getInt(std::string key);
    double getDouble(std::string key);


    // Registers a handler 
    // Hanlder gets called when a setting changes
    // who has the group_tag assigned to it.
    void registerHandler(SettingTag group_tag, std::function<void()> func);
    void unregisterHandler(SettingTag group_tag);
    void unregisterHandlers();

    // Trys to load the settings from a .json file
    // which file path is defined in DEFINITIONS.hpp
    void loadSettings();
    // Saves the settings to a .json file
    // which file path is defined in DEFINITIONS.hpp
    void saveSettings();
};

#endif
