#ifndef SETTING_HPP
#define SETTING_HPP

// Includes
#include <TGUI/extlibs/Aurora/SmartPtr/ClonersAndDeleters.hpp>
#include <variant>
#include <cstdint>
#include <cmath>
#include <vector>
#include <memory>

#include "Log.hpp"
#include "SettingValidator.hpp"


class Setting{
using Value = std::variant<bool, int, double>;
public:
    enum class Type {Bool, Int, Double};
    enum class Tag {GRAPHICS, GAMEPLAY};

    class ValueValidator{
    public:
        virtual bool isValid(const Value& value) const = 0;
        virtual bool isType(const Type& type) const = 0;
        virtual Value getClosestValidValue(const Value& value) = 0;
    };

    class RangeValueValidator : public ValueValidator{
    public:
        struct Range{
            enum class BoundType{
                CLOSED,
                OPEN,
            };

            Value lower;
            Value upper;
            BoundType lower_bound_type;
            BoundType upper_bound_type;

            Range(Value t_lower, Value t_upper, BoundType t_lower_bound_type, BoundType t_upper_bound_type);
            Range(Value t_lower, Value t_upper);
            
            bool contains(const Value& value) const;
        };

        RangeValueValidator(const std::vector<Range>& t_ranges);
        bool isValid(const Value& value) const override;
        bool isType(const Type& type) const override;
        std::vector<Range> getRanges();
    private:
        std::vector<Range> ranges;

    };

    class DiscreteValueValidator : public ValueValidator{
    private:
        std::vector<Value> valid_values;
    public:
        DiscreteValueValidator(const std::vector<Value>& t_valid_values);
        bool isValid(const Value& value) const override;
        bool isType(const Type& type) const override;
        std::vector<Value> getValidValues();
    };
    Setting(Tag t_tag, Value t_value, std::unique_ptr<ValueValidator> t_validator);
    std::string valueToString(Value value);

    inline void setValue(Value t_value){
        if(static_cast<Type>(t_value.index()) != getType()) {
            throw std::runtime_error("Setting - setValue - Tried setting wrong type. Previous value: " + std::to_string(value.index()) + " new type: " + std::to_string(t_value.index()));
        }

        if(validator->isValid(t_value)){
            value = t_value;
        }
        else {
            CORE_INFO("ConfigurationManager - setValue - value: {} is not valid", valueToString(t_value));
            value = validator->getClosestValidValue(t_value);
        }
    }

    inline Tag getTag(){
        return tag;
    }

    inline Type getType() const {
        return static_cast<Type>(value.index());
    }

    inline Value getValue() const {
        return value;
    }   
private:
    Tag tag;
    Value value;
    std::unique_ptr<ValueValidator> validator;
};

#endif
