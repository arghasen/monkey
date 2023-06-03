#include "object.hpp"

namespace monkey::evaluator {

Integer::Integer(int value) : value_(value) {}

std::string Integer::to_string() const { return std::to_string(value_); }

std::string Integer::type() const { return INTEGER_OBJ; }

Boolean::Boolean(bool value) : value_(value) {}

std::string Boolean::to_string() const { return value_ ? "true" : "false"; }

std::string Boolean::type() const { return BOOLEAN_OBJ; }

std::string Null::to_string() const { return "null"; }

std::string Null::type() const { return NULL_OBJ; }

ReturnValue::ReturnValue(ObjectPtr value) : value_(std::move(value)) {}

std::string ReturnValue::to_string() const { return value_->to_string(); }

std::string ReturnValue::type() const { return RETURN_VALUE_OBJ; }


} // namespace monkey::evaluator
