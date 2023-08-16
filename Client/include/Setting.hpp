#ifndef SETTING_HPP
#define SETTING_HPP

// Includes
#include <variant>
#include <cstdint>
#include <cmath>

#include "Log.hpp"

enum class SettingTag{
    GRAPHICS,
    GAMEPLAY,
};

class Setting{
using Value = std::variant<bool, int, double>;
public:
    enum class Type {Bool, Int, Double};

    Setting(SettingTag t_tag, Value t_value, Value t_min_value, Value t_max_value) : 
        tag(t_tag),
        min_value(t_min_value), 
        max_value(t_max_value)
        {
            if(!(t_value.index() == t_min_value.index() && t_min_value.index() == t_max_value.index())) {
                throw std::runtime_error("Setting - Constructor - Tried setting wrong type");
            }
            value = t_value;
            setValue(t_value);
        };
    Setting(): tag(SettingTag::GRAPHICS),  value(false), min_value(false), max_value(true) {};

    std::string valueToString(Value value){
        switch (static_cast<Type>(value.index())){
            case Type::Bool:
                return std::to_string(std::get<bool>(value));
            case Type::Int:
                return std::to_string(std::get<int>(value));
            case Type::Double:
                return std::to_string(std::get<double>(value));
        }
        return "";
    }

    void setValue(Value t_value){
        if(static_cast<Type>(t_value.index()) != getType()) {
            throw std::runtime_error("Setting - setValue - Tried setting wrong type. Previous value: " + std::to_string(value.index()) + " new type: " + std::to_string(t_value.index()));
        }

        if(t_value >= min_value && t_value <= max_value){
            value = t_value;
        }
        else {
            CORE_INFO("ConfigurationManager - out_of_range value: {}, min: {}, max:{}", valueToString(t_value), valueToString(min_value), valueToString(max_value));
            value = std::max(min_value, std::min(t_value, max_value));
        }
    }

    SettingTag getTag(){
        return tag;
    }

    Type getType() const {
        return static_cast<Type>(value.index());
    }

    Value getValue() const {
        return value;
    }

    Value getMin() const {
        return min_value;
    }

    Value getMax() const {
        return max_value;
    }   
private:
    SettingTag tag;
    Value value;
    Value min_value;
    Value max_value;
};

#endif
