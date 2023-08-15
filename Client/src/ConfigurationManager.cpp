#include "ConfigurationManager.hpp"
#include <json/json.h>

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

void loadSettings(){
}

void saveSettings(){
}
