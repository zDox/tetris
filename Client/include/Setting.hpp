#ifndef SETTING_HPP
#define SETTING_HPP

// Includes
#include <TGUI/extlibs/Aurora/SmartPtr/ClonersAndDeleters.hpp>
#include <variant>
#include <cstdint>
#include <cmath>
#include <vector>
#include <memory>
#include <vector>
#include <string> 
#include <json/json.h>
#include <utility>

#include "Log.hpp"

enum class SettingTag{
    GRAPHICS,
    GAMEPLAY
};

static const std::vector<std::string> REQUIRED_SETTING_MEMBERS {"type", "default_value", "validator"};

template <typename T>
concept AllowedTypes = std::is_same_v<T, int> ||
                      std::is_same_v<T, bool> ||
                      std::is_same_v<T, double> ||
                      std::is_same_v<T, float>;
template 
<AllowedTypes T>
class Setting{
public:
    static SettingTag parseTag(std::string tag_string){
        if(tag_string == "GRAPHICS"){
            return SettingTag::GRAPHICS;
        }
        else if(tag_string == "GAMEPLAY"){
            return SettingTag::GAMEPLAY;
        }
        else {
            throw std::runtime_error("Invalid setting tag: " + tag_string);
        }
    }

    static Setting parseSetting(Json::Value obj){
        SettingTag tag = parseTag(obj["tag"].asString());
        T default_value = obj["default_value"].as<T>();
        
        return Setting<T>(tag, default_value, default_value); 
    }

    Setting(SettingTag t_tag, T t_value, T t_default_value) :
        tag(t_tag){
            value = t_value;
            setValue(t_value);
    }
    
    // Default constructor
    Setting(){
    }

    // Copy constructor
    Setting(const Setting& other) :
        tag(other.tag),
        value(other.value),
        default_value(other.default_value){
    }

    void reset(){
        setValue(default_value);
    }

    void setValue(T t_value){
        value = t_value;
    }

    SettingTag getTag(){
        return tag;
    }

    T getValue() const {
        return value;
    }   

private:
    SettingTag tag;
    T value;
    T default_value;
};

#endif
