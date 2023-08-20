#include "ConfigurationManager.hpp"
#include <json/json.h>
#include <fstream>

#include "Definitions.hpp"

void Config::resetSetting(std::string key){
    if(settings.contains(key)){
        std::visit([](auto&& setting){
            setting.reset();
        }, settings[key]);
    }
    else {
        throw std::runtime_error("ConfigurationManager - resetSetting - Setting '" + key + "' is not valid.");
    }
}

// Setter

void Config::setBool(std::string key, bool value){
    setValue<bool>(key, value);
}

void Config::setInt(std::string key, int value){
    setValue<int>(key, value);
}

void Config::setDouble(std::string key, double value){
    setValue<double>(key, value);
}

// Getter

bool Config::getBool(std::string key){
    return getValue<bool>(key);
}

int Config::getInt(std::string key){
    return getValue<int>(key);
}

double Config::getDouble(std::string key){
    return getValue<double>(key);
}
void Config::registerHandler(SettingTag group_tag, std::function<void()> func){
    if(handlers.contains(group_tag)){
        handlers[group_tag] = func;
    }
    else {
        handlers.emplace(group_tag, func);
    }
}

void Config::unregisterHandler(SettingTag group_tag){
    if(!handlers.contains(group_tag)) return;
    handlers.erase(group_tag);
}

void Config::unregisterHandlers(){
    handlers.clear();
}

void Config::loadSettings(std::string file_path){
    std::ifstream file(file_path);
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

        switch (d_value.index()){
            case 0:
            {
                std::get<Setting<bool>>(settings[key]).setValue(root[key].asBool());
                break;
            }
            case 1:
            {
                std::get<Setting<int>>(settings[key]).setValue(root[key].asInt());
                break;
            }
            case 2:
            {
                std::get<Setting<double>>(settings[key]).setValue(root[key].asDouble());
                break;
            }
        }
    }
    file.close();
}

void Config::loadSettingDetails(std::string file_path){
    std::ifstream file(file_path);
    if(!file.is_open()){
        throw std::runtime_error("ConfigurationManager - SettingDetails '" + file_path + "' file not found");
    }

    Json::Reader reader;
    Json::Value root;
    std::string errs;
    if(!reader.parse(file, root)) {
        throw std::runtime_error("ConfigurationManager - Failed parsing configuration file: " + errs);
    }

    
    for (const auto& key : root.getMemberNames()){
        bool skip = false;
        for(const auto& key : REQUIRED_SETTING_MEMBERS){
            if(!root.isMember(key)) skip = true;
        }
        
        SettingType type = stringToType(root[key]["type"].asString());

        if(skip) continue;
    
    file.close();
}
void Config::saveSettings(){
    Json::Value root;
    for(auto [key, value] : settings){
        switch(value.index()){
            case 0:
                root[key] = std::get<Setting<bool>>(value).getValue();
                break;

            case 1:
                root[key] = std::get<Setting<int>>(value).getValue();
                break;

            case 2:
                root[key] = std::get<Setting<double>>(value).getValue();
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

void Config::load(std::string file_settings, std::string file_setting_details){
    loadSettingDetails(file_setting_details);
    loadSettings(file_settings);
}
