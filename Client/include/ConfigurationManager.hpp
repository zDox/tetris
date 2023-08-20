#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>

#include "Log.hpp"
#include "Setting.hpp"
#include "DefaultConfig.hpp"


class Config{
using SettingWrapper = std::variant<Setting<bool>, Setting<int>, Setting<double>>;
enum class SettingType {BOOL, INT, DOUBLE};
SettingType stringToType(std::string type_string){
    if (type_string == "bool") {
        return SettingType::BOOL;
    } else if (type_string == "int") {
        return SettingType::INT;
    } else if (type_string == "double") {
        return SettingType::DOUBLE;
    } else {
        // Handle the case when the input string is not a valid type
        throw std::runtime_error("Invalid setting type: " + type_string);
    }
}
private:
    std::unordered_map<std::string, SettingWrapper> settings;
    std::unordered_map<SettingTag, std::function<void()>> handlers;

    void resetSetting(std::string key);

    template <typename T>
    void setValue(std::string key, T value){
        if(settings.contains(key)) {
            try {
                std::get<Setting<T>>(settings[key]).setValue(value);
                if(handlers.contains(std::get<Setting<T>>(settings[key]).getTag())) {
                    handlers[std::get<Setting<T>>(settings[key]).getTag()]();
                }
            }
            catch (std::bad_variant_access const& e){
                throw std::runtime_error("ConfigurationManager - setValue - Setting is of type: " + std::to_string(settings[key].index()));
            }
        }
        else{
            throw std::runtime_error("ConfigurationManager - setValue - Setting '" + key + "' is not valid.");
        }
    }

    template <typename T>
    T getValue(std::string key){
        if(settings.contains(key)){
            try {
                return std::get<Setting<T>>(settings[key]).getValue();
            }
            catch (std::bad_variant_access const& e){
                throw std::runtime_error("ConfigurationManager - getValue - Setting is of type: " + std::to_string(settings[key].index()));
            }
        }
        else {
            throw std::runtime_error("ConfigurationManager - getValue - Setting '" + key + "' is not valid"); 
        }
    }

    // Trys to load the settings from a .json file
    // which file path is defined in DEFINITIONS.hpp
    void loadSettings(std::string file_path);
    void loadSettingDetails(std::string file_path);
    void saveSettings();

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

    void load(std::string file_settings, std::string file_setting_details);
    // Saves the settings to a .json file
    // which file path is defined in DEFINITIONS.hpp
    void save();
};

#endif
