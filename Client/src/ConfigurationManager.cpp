#include "ConfigurationManager.hpp"
#include <json/json.h>
#include <fstream>

#include "Definitions.hpp"

void ConfigurationManager::resetSetting(std::string key){
    if(settings.contains(key) && DEFAULT_CONFIG.contains(key)){
        // Setting copy = DEFAULT_CONFIG[key];
        // settings[key] = copy;
    }
    else if(!settings.contains(key)){
        // settings.emplace(key, DEFAULT_CONFIG[key]);
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
void ConfigurationManager::registerGraphicsHandler(std::function<void()> func){
    graphics_handler = func;
}

void ConfigurationManager::loadSettings(){
    std::ifstream file(CONFIG_FILE_NAME);
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
                Setting new_setting(d_value.getTag(), root[key].asBool(), d_value.getMin(), d_value.getMax());
                settings.emplace(key, new_setting);
                break;
            }
            case Setting::Type::Int:
            {
                Setting new_setting(d_value.getTag(), root[key].asInt(), d_value.getMin(), d_value.getMax());
                settings.emplace(key, new_setting);
                break;
            }
            case Setting::Type::Double:
            {
                Setting new_setting(d_value.getTag(), root[key].asDouble(), d_value.getMin(), d_value.getMax());
                settings.emplace(key, new_setting);
                break;
            }
        }
    }
    file.close();
}

void ConfigurationManager::saveSettings(){
    Json::Value root;
    for(auto [key, value] : DEFAULT_CONFIG){
        if(settings.contains(key)){
            // value = settings[key];
        }
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
