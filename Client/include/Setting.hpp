#ifndef SETTING_HPP
#define SETTING_HPP

// Includes
#include <variant>
#include <cstdint>
#include <cmath>

enum class SettingTag{
    GRAPHICS,
    GAMEPLAY,
};

class Setting{
public:
    enum class Type {Bool, Int, Double};

    template <typename T>
    Setting(SettingTag t_tag, T t_value, T t_min_value, T t_max_value) : 
        tag(t_tag),
        min_value(t_min_value), 
        max_value(t_max_value)
        {
            setValue(t_value);
        };


    template <typename T> 
    void setValue(T t_value){
        if(t_value >= std::get<T>(min_value) && t_value <= std::get<T>(max_value)){
            value = t_value;
        }
        else {
            CORE_INFO("ConfigurationManager - out_of_range value: {}, min: {}, max:{}", t_value, std::get<T>(min_value), std::get<T>(max_value));
            value = std::max(std::get<T>(min_value), std::min(t_value, std::get<T>(max_value)));
        }
    }

    SettingTag getTag(){
        return tag;
    }

    Type getType() const {
        return static_cast<Type>(value.index());
    }

    template <typename T>
    T getValue() const {
        return std::get<T>(value);
    }

    template <typename T>
    T getMin() const {
        return std::get<T>(min_value);
    }

    template <typename T>
    T getMax() const {
        return std::get<T>(max_value);
    }   
private:
    SettingTag tag;
    std::variant<bool, std::int64_t, double> value;
    std::variant<bool, std::int64_t, double> min_value;
    std::variant<bool, std::int64_t, double> max_value;
};

#endif
