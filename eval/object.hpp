#pragma once
#include <string>
#include <memory>

namespace monkey {
namespace evaluator {

using OBJECT_TYPE = const char *;
constexpr OBJECT_TYPE INTEGER_OBJ = "INTEGER";
constexpr OBJECT_TYPE BOOLEAN_OBJ = "BOOLEAN";
constexpr OBJECT_TYPE NULL_OBJ = "NULL";
constexpr OBJECT_TYPE RETURN_VALUE_OBJ = "RETURN_VALUE";
constexpr OBJECT_TYPE ERROR_OBJ = "ERROR";
constexpr OBJECT_TYPE FUNCTION_OBJ = "FUNCTION";

class Object {
public:
  virtual ~Object() = default;
  virtual std::string to_string() const = 0;
  virtual std::string type() const = 0;
};

using ObjectPtr = std::shared_ptr<Object>;

class Integer : public Object {
public:
  explicit Integer(int value);
  ~Integer() override = default;
  std::string to_string() const override;
  std::string type() const override;
  int value_;
};

class Boolean : public Object {
public:
  explicit Boolean(bool value);
  ~Boolean() override = default;
  std::string to_string() const override;
  std::string type() const override;
  bool value_;
};

class Null : public Object {
public:
  Null() = default;
  ~Null() override = default;
  std::string to_string() const override;
  std::string type() const override;
};

class ReturnValue : public Object {
public:
  explicit ReturnValue(ObjectPtr value);
  ~ReturnValue() override = default;
  std::string to_string() const override;
  std::string type() const override;
  ObjectPtr value_;
};

class Error : public Object {
public:
  explicit Error(std::string message);
  ~Error() override = default;
  std::string to_string() const override;
  std::string type() const override;
  std::string message_;
};

// class Function : public Object {
// public:
//   Function(std::vector<std::string> parameters, std::string body);
//   ~Function() override = default;
//   std::string to_string() const override;
//   std::string type() const override;
//   std::vector<std::string> parameters_;
//   std::string body_;
//   ObjectPtr env_;
// };

} // namespace evaluator
} // namespace monkey
