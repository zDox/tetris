#include <json/json.h>
#include <fstream>

#include "Definitions.hpp"
#include "ConfigurationManager.hpp"

void ConfigurationManager::resetSetting(std::string key){
    if(settings.contains(key) && DEFAULT_CONFIG.contains(key)){
        settings[key] = DEFAULT_CONFIG[key];
    }
    else if(!settings.contains(key)){
        settings.emplace(key, DEFAULT_CONFIG[key]);
    }
}


// Setter
void ConfigurationManager::setBool(std::string key, bool value){
    setSetting(key, value);
}

void ConfigurationManager::setInt(std::string key, int value){
    setSetting(key, value);
}

void ConfigurationManager::setDouble(std::string key, double value){
    setSetting(key, value);
}

// Getter
bool ConfigurationManager::getBool(std::string key){
    return getSetting<bool>(key);
}

int ConfigurationManager::getInt(std::string key){
    return getSetting<std::int64_t>(key);
}

void ConfigurationManager::registerGraphicsHandler(std::function<void()> func){
    graphics_handler = func;
}

void ConfigurationManager::loadSettings(){
    std::ifstream file(CONFIG_FILE_PATH);
    if(!file.is_open()){
        CORE_WARN("ConfigurationManager - Config file not found. Using default configuration.");
        settings = DEFAULT_CONFIG;
        return;
    }

    Json::CharReaderBuilder reader_builder;
    Json::Value root;
    std::string errs;
    if(!Json::parseFromStream(reader_builder, file, &root, &errs)) {
        CORE_WARN("ConfigurationManager - Failed parsing configuration file: {}", errs);
        CORE_WARN("ConfigurationManager - using default configuration");
        settings = DEFAULT_CONFIG;
        return;
    }
    
    for(auto [key, d_value] : DEFAULT_CONFIG){
        if(!root.isMember(key)){
            CORE_WARN("ConfigurationManager - Key: {} not found in config file. Using default", key);
            settings.emplace(key, d_value);
            continue;
        }

        switch (d_value.getType()){
            case Setting::Type::Bool:
            {
                Setting new_setting(d_value.getTag(), root[key].asBool(), d_value.getMin<bool>(), d_value.getMax<bool>());
                settings.emplace(key, new_setting);
                break;
            }
            case Setting::Type::Int:
            {
                Setting new_setting(d_value.getTag(), root[key].asInt(), d_value.getMin<int>(), d_value.getMax<int>());
                settings.emplace(key, new_setting);
            }
            case Setting::Type::Double:
            {
                Setting new_setting(d_value.getTag(), root[key].asDouble(), d_value.getMin<double>(), d_value.getMax<double>());
                settings.emplace(key, new_setting);
            }
        }
    }
}

void ConfigurationManager::saveSettings(){
    Json::Value root;
    for(auto [key, d_value] : DEFAULT_CONFIG){
        if(settings.contains(key)){
        
}
