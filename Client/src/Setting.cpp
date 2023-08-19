#include "Setting.hpp"

using Value = std::variant<bool, int, double>;

// Range implementation for RangeValueValidator
Setting::RangeValueValidator::Range::Range(Value t_lower, Value t_upper, BoundType t_lower_bound_type, BoundType t_upper_bound_type) : 
    lower(t_lower), upper(t_upper), lower_bound_type(t_lower_bound_type), upper_bound_type(t_upper_bound_type){};

Setting::RangeValueValidator::Range::Range(Value t_lower, Value t_upper){
    Setting::RangeValueValidator::Range(t_lower, t_upper, BoundType::CLOSED, BoundType::CLOSED);
};

bool Setting::RangeValueValidator::Range::contains(const Value& value)const{
    bool lower_bound_check = (lower_bound_type == BoundType::CLOSED) ? (value >= lower) : true;
    bool upper_bound_check = (upper_bound_type == BoundType::CLOSED) ? (value <= upper) : true;
    return (lower_bound_check && upper_bound_check);
}

// RangeValueValidator implementation
Setting::RangeValueValidator::RangeValueValidator(const std::vector<Range>& t_ranges){
    for(int i = 0; i < t_ranges.size()-1; i++){
        if(t_ranges[i].upper_bound_type != t_ranges[i+1].lower_bound_type){
            CORE_WARN("Setting - RangeValueValidator - Upper bound of Range({}) is overlapping with lower bound of Range({})",i , i+1);
        }
    }
}

bool Setting::RangeValueValidator::isValid(const Value& value) const{
    for(const Range& range: ranges){
        if(range.contains(value)){
            return true;
        }
    }
    return false;
}

bool Setting::RangeValueValidator::isType(const Type& type) const{
    for(const auto& range : ranges){
        if(static_cast<Type>(range.lower.index()) != type || 
           static_cast<Type>(range.upper.index()) != type){
            return false;
        }
    }
    return true;
}

std::vector<Setting::RangeValueValidator::Range> Setting::RangeValueValidator::getRanges(){
    return ranges;
}

// DiscreteValueValidator implementation
Setting::DiscreteValueValidator::DiscreteValueValidator(const std::vector<Value>& t_valid_values) : valid_values(t_valid_values) {};

bool Setting::DiscreteValueValidator::isValid(const Value& value) const {
    for(const Value& c_value : valid_values){
        if(c_value != value) return false;
    }
    return true;
}

bool Setting::DiscreteValueValidator::isType(const Type& type) const {
    for(const Value& value : valid_values){
        if(static_cast<Type>(value.index()) != type) return false;
    }
    return true;
}

std::vector<Value> Setting::DiscreteValueValidator::getValidValues(){
    return valid_values;
}

// Setting implementation
Setting::Setting(Tag t_tag, Value t_value, std::unique_ptr<ValueValidator> t_validator) : 
    tag(t_tag), validator(std::move(t_validator)) {
        if(!(validator->isType(getType()))) {
            throw std::runtime_error("Setting - Constructor - Tried setting wrong type");
        }
        value = t_value;
        setValue(t_value);
};

std::string Setting::valueToString(Value value){
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
