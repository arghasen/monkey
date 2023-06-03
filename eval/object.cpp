#include "object.hpp"
#include <sstream>

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

std::string Error::to_string() const { return message_; }

std::string Error::type() const { return ERROR_OBJ; }

Error::Error(std::string message) : message_(std::move(message)) {}

Function::Function(parser::ast::Parameters params, std::unique_ptr<parser::ast::BlockStatement> bod,
                   Environment* env)
    : parameters(std::move(params)), body(std::move(bod)),
      env_(std::move(env)) {}

std::string Function::to_string() const {
    std::ostringstream oss;
    oss << "fn(";
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        oss << (*it)->to_string();
        if (std::next(it) != parameters.end()) {
        oss << ", ";
        }
    }
    oss << ") {\n";
    oss << body->to_string();
    oss << "\n}";
    return oss.str();
}

std::string Function::type() const { return FUNCTION_OBJ; }

Environment::StoreData Environment::get(const std::string &name) {
  auto it = store_.find(name);
  if (it != store_.end()) {
    return StoreData{.value = it->second, .found = true};
  }
  return StoreData{.value = nullptr, .found = false};
}

void Environment::set(const std::string &name, ObjectPtr value) {
  store_.insert_or_assign(name, std::move(value));
}

} // namespace monkey::evaluator
