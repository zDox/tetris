#ifndef CONFIGURATION_MANAGER_HPP
#define CONFIGURATION_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
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
