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

#include "Log.hpp"

enum class SettingTag{
    GRAPHICS,
    GAMEPLAY
};

static const std::vector<std::string> REQUIRED_SETTING_MEMBERS {"type", "default", "validator"};

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
            enum class BoundType{
                CLOSED,
                OPEN,
            };

            T lower;
            T upper;
            BoundType lower_bound_type;
            BoundType upper_bound_type;

            Range(T t_lower, T t_upper, 
                    BoundType t_lower_bound_type, BoundType t_upper_bound_type) : 
                lower(t_lower), upper(t_upper), lower_bound_type(t_lower_bound_type), 
                upper_bound_type(t_upper_bound_type){
                };
            Range(T t_lower, T t_upper){
                Setting::RangeValueValidator::Range(t_lower, t_upper, 
                                                    BoundType::CLOSED, BoundType::CLOSED);
            }
 
            bool contains(const T& value) const {
                bool lower_bound_check = (lower_bound_type == BoundType::CLOSED) ? 
                                         (value >= lower) : true;
                bool upper_bound_check = (upper_bound_type == BoundType::CLOSED) ? 
                                         (value <= upper) : true;
                return (lower_bound_check && upper_bound_check);
            }
        };

        RangeValueValidator(const std::vector<Range>& t_ranges){
            for(size_t i = 0; i < t_ranges.size()-1; i++){
                if(t_ranges[i].upper_bound_type != t_ranges[i+1].lower_bound_type){
                    CORE_WARN("Setting - RangeValueValidator - Upper bound of Range({}) is overlapping with lower bound of Range({})",i , i+1);
                }
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

    Setting(SettingTag t_tag, T t_value, T t_default_value, std::shared_ptr<ValueValidator> t_validator) :
        tag(t_tag), validator(t_validator) {
            value = t_value;
            setValue(t_value);
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
