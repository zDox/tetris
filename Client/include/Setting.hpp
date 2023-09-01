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

template <AllowedTypes T>
class ValueValidator{
public:
    virtual bool isValid(const T& value) const = 0;
    virtual T getClosestValidValue(const T& value) const = 0;
};

template <AllowedTypes T>
class RangeValueValidator : public ValueValidator<T>{
    bool contains(const T& value) const {
        return (lower <= value && value <= upper);
    }
public:
    RangeValueValidator(const std::pair<T, T>& t_ranges){
        if(t_ranges[0] > t_ranges[1]) return;

        lower = t_ranges.first;
        upper = t_ranges.second;
    };

    bool isValid(const T& value) const override{
        return contains(value);
    };

    T getClosestValidValue(const T& value) const override {
        T min_distance = std::numeric_limits<T>::max();
        int closest_index = -1;


        if(contains(value)){
            return value;
        }

        T lower_dist = abs(lower - value);
        if (lower_dist < min_distance) {
            min_distance = lower_dist;
            closest_index = 0;
        }

        T upper_dist = abs(upper - value);
        if (upper_dist < min_distance) {
            min_distance = upper_dist;
            closest_index = 1;
        }

        if (closest_index >= 0) {
            if (closest_index % 2 == 0) {
                return lower;
            } else {
                return upper;
            }
        } else {
            // This fallback might need adjustment 
            return value; // Return the original value if no valid range found
        }        
    }

    T getMin(){
        return lower;
    }

    T getMax(){
        return upper;
    }

private:
    T lower;
    T upper;

};

template <AllowedTypes T>
class DiscreteValueValidator : public ValueValidator<T>{
private:
    std::vector<T> valid_values;
public:
    DiscreteValueValidator(const std::vector<T>& t_valid_values) :
        valid_values(t_valid_values) {};

    bool isValid(const T& value) const override {
        for(const T& c_value : valid_values){
            if(c_value == value) return true;
        }
        return false;
    };

    T getClosestValidValue(const T& value) const override{
        T closest_value = valid_values[0];
        T min_distance = std::numeric_limits<T>::max();

        for (const T& c_value : valid_values) {
            T distance = std::abs(c_value - value);
            if (distance < min_distance) {
                min_distance = distance;
                closest_value = c_value;
            }
            // No need to further search because a distance smaller than 0 is not possible
            if(distance == 0) break; 
        }

        return closest_value;
    }

    std::vector<T> getValidValues(){
        return valid_values;
    }
};


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

    static Setting parseSetting(std::string name, Json::Value obj){
        SettingTag tag = parseTag(obj["tag"].asString());
        T default_value = obj["default_value"].as<T>();

        // parsing Validator
        std::unique_ptr<ValueValidator<T>> validator = nullptr;
        Json::Value val_obj = obj["validator"];

        if(val_obj["type"] == "range"){
            std::vector<typename RangeValueValidator<T>::Range> vec;
            Json::Value range = val_obj["range"];
            if(range.size() == 2){
                vec.push_back(typename RangeValueValidator<T>::Range(range[0].as<T>(),
                                                                     range[1].as<T>()));
            }

            validator = std::make_unique<RangeValueValidator<T>>(std::as_const(vec));
        }
        else if(val_obj["type"] == "discrete"){
            std::vector<T> vec;
            for(auto const& valid_value : val_obj["valid_values"]){
                vec.push_back(valid_value.as<T>());
            }

            validator = std::make_unique<DiscreteValueValidator<T>>(std::as_const(vec));
        }
        else throw std::runtime_error("ConfigurationManager - parseValidator - No type of Validator specified");

        return Setting<T>(name, tag, default_value, default_value, std::move(validator)); 
    }

    Setting(std::string t_name, SettingTag t_tag, T t_value, T t_default_value, std::unique_ptr<ValueValidator<T>> t_validator) :
        name(t_name), tag(t_tag), validator(std::move(t_validator)){
            value = t_value;
            setValue(t_value);
    }
    
    // Default constructor
    Setting(){
    }


    void reset(){
        setValue(default_value);
    }

    void setValue(T t_value){
        if(validator->isValid(t_value)){
            value = t_value;
        }
        else {
            value = validator->getClosestValidValue(t_value);
        }
    }

    SettingTag getTag(){
        return tag;
    }

    T getValue() const {
        return value;
    }   

private:
    std::string name;
    SettingTag tag;
    T value;
    T default_value;
    std::unique_ptr<ValueValidator<T>> validator;
};

#endif
