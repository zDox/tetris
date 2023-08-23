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
    class ValueValidator{
    public:
        struct Range{
            T lower;
            T upper;

            Range(T t_lower, T t_upper) : 
                lower(t_lower), upper(t_upper){
            };
 
            bool contains(const T& value) const {
                return (lower <= value && value <= upper);
            }
        };

        ValueValidator(const std::vector<Range>& t_ranges){
            for(size_t i = 0; i < t_ranges.size()-1; i++){
                if(t_ranges[i].upper > t_ranges[i+1].lower){
                    CORE_WARN("Setting - RangeValueValidator - Upper bound of Range({}) is overlapping with lower bound of Range({})",i , i+1);
                }
                ranges.push_back(t_ranges[i]);
            }
        };

        ValueValidator(){
        };

        bool isValid(const T& value) const{
            for(const Range& range: ranges){
                if(range.contains(value)){
                    return true;
                }
            }
            return false;
        };
 
        T getClosestValidValue(const T& value) const{
            T min_distance = std::numeric_limits<T>::max();
            int closest_index = -1;

            for (size_t i = 0; i < ranges.size(); ++i) {

                if(ranges[i].contains(value)){
                    return value;
                }

                T lower_dist = abs(ranges[i].lower - value);
                if (lower_dist < min_distance) {
                    min_distance = lower_dist;
                    closest_index = static_cast<int>(i * 2);
                }

                T upper_dist = abs(ranges[i].upper - value);
                if (upper_dist < min_distance) {
                    min_distance = upper_dist;
                    closest_index = static_cast<int>(i * 2 + 1);
                }
            }

            if (closest_index >= 0) {
                if (closest_index % 2 == 0) {
                    return ranges[closest_index / 2].lower;
                } else {
                    return ranges[closest_index / 2].upper;
                }
            } else {
                // This fallback might need adjustment 
                return value; // Return the original value if no valid range found
            }        
        }

        std::vector<Range> getRanges(){
            return ranges;
        }

    private:
        std::vector<Range> ranges;
    };

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
        
        Setting<T>::ValueValidator validator = parseValidator(obj["validator"]);
        return Setting<T>(tag, default_value, validator); 
    }

    static Setting<T>::ValueValidator parseValidator(Json::Value obj){
        if(obj["type"] == "range"){
            std::vector<typename Setting<T>::ValueValidator::Range> vec;
            for(auto const& range : obj["ranges"]){
                if(range.size() == 2){
                    vec.push_back(typename Setting<T>::ValueValidator::Range(range[0].as<T>(),
                                                                         range[1].as<T>()));
                }
            }

            return Setting<T>::ValueValidator(std::as_const(vec));
        }
        else if(obj["type"] == "discrete"){
            std::vector<typename Setting<T>::ValueValidator::Range> vec;
            for(auto const& valid_value : obj["valid_values"]){
                vec.push_back(typename Setting<T>::ValueValidator::Range(valid_value.as<T>(),
                                                                         valid_value.as<T>()));
            }

            return Setting<T>::ValueValidator(std::as_const(vec));
        }
        else throw std::runtime_error("ConfigurationManager - parseValidator - No type of Validator specified");
    }

    Setting(SettingTag t_tag, T t_value, T t_default_value, ValueValidator t_validator) :
        tag(t_tag) {
            value = t_value;
            validator = t_validator;
            setValue(t_value);
    }

    Setting(SettingTag t_tag, T t_default_value, ValueValidator t_validator) { 
        Setting(t_tag, t_default_value, t_default_value, t_validator);
    }

    Setting(){
    }

    void reset(){
        setValue(default_value);
    }

    void setValue(T t_value){
        if(validator.isValid(t_value)){
            value = t_value;
        }
        else {
            value = validator.getClosestValidValue(t_value);
            CORE_INFO("ConfigurationManager - setValue - value: {} is not valid. Falling back to: {}", std::to_string(t_value), std::to_string(value));
        }
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
    ValueValidator validator;
};

#endif
