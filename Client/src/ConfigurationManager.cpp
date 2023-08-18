#include "ConfigurationManager.hpp"
#include <json/json.h>
#include <fstream>

#include "Definitions.hpp"

void ConfigurationManager::resetSetting(std::string key){
    if(settings.contains(key)){
        settings[key] = DEFAULT_CONFIG[key];
    }
    else {
        throw std::runtime_error("ConfigurationManager - resetSetting - Setting '" + key + "' is not valid.");
    }
}

// Setter

void ConfigurationManager::setBool(std::string key, bool value){
    setValue<bool>(key, value);
}

void ConfigurationManager::setInt(std::string key, int value){
    setValue<int>(key, value);
}

void ConfigurationManager::setDouble(std::string key, double value){
    setValue<double>(key, value);
}

// Getter

bool ConfigurationManager::getBool(std::string key){
    return getValue<bool>(key);
}

int ConfigurationManager::getInt(std::string key){
    return getValue<int>(key);
}

double ConfigurationManager::getDouble(std::string key){
    return getValue<double>(key);
}
void ConfigurationManager::registerHandler(SettingTag group_tag, std::function<void()> func){
    if(handlers.contains(group_tag)){
        handlers[group_tag] = func;
    }
    else {
        handlers.emplace(group_tag, func);
    }
}

void ConfigurationManager::unregisterHandler(SettingTag group_tag){
    if(!handlers.contains(group_tag)) return;
    handlers.erase(group_tag);
}

void ConfigurationManager::unregisterHandlers(){
    handlers.clear();
}

void ConfigurationManager::loadSettings(){
    std::ifstream file(CONFIG_FILE_NAME);
    settings = DEFAULT_CONFIG;
    if(!file.is_open()){
        CORE_WARN("ConfigurationManager - Config file not found. Using default configuration.");
        return;
    }

    Json::CharReaderBuilder reader_builder;
    Json::Value root;
    std::string errs;
    if(!Json::parseFromStream(reader_builder, file, &root, &errs)) {
        CORE_WARN("ConfigurationManager - Failed parsing configuration file: {}", errs);
        CORE_WARN("ConfigurationManager - using default configuration");
        return;
    }
    
    for(auto [key, d_value] : DEFAULT_CONFIG){
        if(!root.isMember(key)){
            CORE_WARN("ConfigurationManager - Key: {} not found in config file. Using default", key);
            continue;
        }

        switch (d_value.getType()){
            case Setting::Type::Bool:
            {
                settings[key].setValue(root[key].asBool());
                break;
            }
            case Setting::Type::Int:
            {
                settings[key].setValue(root[key].asInt());
                break;
            }
            case Setting::Type::Double:
            {
                settings[key].setValue(root[key].asDouble());
                break;
            }
        }
    }
    file.close();
}

void ConfigurationManager::saveSettings(){
    Json::Value root;
    for(auto [key, value] : settings){
        switch(value.getType()){
            case Setting::Type::Bool:
                root[key] = std::get<bool>(value.getValue());
                break;

            case Setting::Type::Int:
                root[key] = std::get<int>(value.getValue());
                break;

            case Setting::Type::Double:
                root[key] = std::get<double>(value.getValue());
                break;
        }
    }

    std::ofstream file(CONFIG_FILE_NAME);
    if(!file.is_open()){
        CORE_WARN("ConfigurationManager - writing - Failed to open file");
        return;
    }

    file << root;
    CORE_DEBUG("ConfigurationManager - writing - Saved config file");
    file.close();
}
