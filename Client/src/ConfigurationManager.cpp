#include "ConfigurationManager.hpp"
#include <json/json.h>
#include <fstream>
#include <filesystem>

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
    
    for(auto [key, d_value] : settings){
        if(!root.isMember(key)){
            CORE_WARN("ConfigurationManager - Key: {} not found in config file. Falling back to default.", key);
            continue;
        }
        CORE_DEBUG("Key: {}", key);

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
        for(const auto& check_key : REQUIRED_SETTING_MEMBERS){
            if(!root[key].isMember(check_key)){
                CORE_WARN("ConfigurationManager - loadSettingDetails - Required member '{}' not in setting '{}'",
                           check_key, key);
                skip = true;
            }
        }
        
        if(skip) continue;
        SettingType type = parseType(root[key]["type"].asString());

        SettingWrapper setting;
        switch (type){
            case SettingType::BOOL:
                setting = Setting<bool>::parseSetting(root[key]); 
                break;
            case SettingType::INT: 
                setting = Setting<int>::parseSetting(root[key]);
                break;
            case SettingType::DOUBLE:
                setting = Setting<double>::parseSetting(root[key]);
                break;
            default:
                continue;
        }
        settings.emplace(key, std::move(setting));
    }

    
    file.close();
}
void Config::saveSettings(std::string file_path){
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

    std::ofstream file(file_path);
    if(!file.is_open()){
        CORE_WARN("ConfigurationManager - writing - Failed to open file");
        return;
    }

    file << root;
    CORE_DEBUG("ConfigurationManager - writing - Saved config file");
    file.close();
}

void Config::load(std::string t_filename_settings, std::string t_filename_settings_details){
    file_path_settings = "res/" + t_filename_settings;
    file_path_settings_details = "res/" + t_filename_settings_details;
    loadSettingDetails(file_path_settings_details);
    loadSettings(file_path_settings);
}

void Config::save(){
    saveSettings(file_path_settings);
}
