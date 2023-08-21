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
        virtual bool isValid(const T& value) const = 0;
        virtual T getClosestValidValue(const T& value) const = 0;
    };

    class RangeValueValidator : public ValueValidator{
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

        RangeValueValidator(const std::vector<Range>& t_ranges){
            for(size_t i = 0; i < t_ranges.size()-1; i++){
                if(t_ranges[i].upper > t_ranges[i+1].lower){
                    CORE_WARN("Setting - RangeValueValidator - Upper bound of Range({}) is overlapping with lower bound of Range({})",i , i+1);
                }
                ranges.push_back(t_ranges[i]);
            }
        };

        bool isValid(const T& value) const override{
            for(const Range& range: ranges){
                if(range.contains(value)){
                    return true;
                }
            }
            return false;
        };
 
        T getClosestValidValue(const T& value) const override {
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

    class DiscreteValueValidator : public ValueValidator{
    private:
        std::vector<T> valid_values;
    public:
        DiscreteValueValidator(const std::vector<T>& t_valid_values) :
            valid_values(t_valid_values) {};

        bool isValid(const T& value) const override {
            for(const T& c_value : valid_values){
                if(c_value != value) return false;
            }
            return true;
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
        
        std::shared_ptr<Setting<T>::ValueValidator> validator = parseValidator(obj["validator"]);
        return Setting<T>(tag, default_value, validator); 
    }

    static std::shared_ptr<Setting<T>::ValueValidator> parseValidator(Json::Value obj){
        std::shared_ptr<Setting<T>::ValueValidator> validator;
        if(obj["type"] == "range"){
            std::vector<typename Setting<T>::RangeValueValidator::Range> vec;
            for(auto const& range : obj["ranges"]){
                if(range.size() == 2){
                    vec.push_back(typename Setting<T>::RangeValueValidator::Range(range[0].as<T>(),
                                                                         range[1].as<T>()));
                }
            }

            validator = std::make_shared<Setting<T>::RangeValueValidator>(std::as_const(vec));
        }
        else if(obj["type"] == "discrete"){
            std::vector<T> vec;
            for(auto const& valid_value : obj["valid_values"]){
                vec.push_back(valid_value.as<T>());
            }

            validator = std::make_shared<Setting<T>::DiscreteValueValidator>(std::as_const(vec));
        }
        else throw std::runtime_error("ConfigurationManager - parseValidator - No type of Validator specified");
        return validator;
    }

    Setting(SettingTag t_tag, T t_value, T t_default_value, std::shared_ptr<ValueValidator> t_validator) :
        tag(t_tag), validator(t_validator) {
            value = t_value;
            setValue(t_value);
    }

    Setting(SettingTag t_tag, T t_default_value, std::shared_ptr<ValueValidator> t_validator) { 
        Setting(t_tag, t_default_value, t_default_value, t_validator);
    }

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
            CORE_INFO("ConfigurationManager - setValue - value: {} is not valid", std::to_string(t_value));
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
    SettingTag tag;
    T value;
    T default_value;
    std::shared_ptr<ValueValidator> validator;
};

#endif
