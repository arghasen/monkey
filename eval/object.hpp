#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "../parser/ast.hpp"

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
using Results = std::vector<ObjectPtr>;

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


class EnvironmentImpl{
public:
  using Store = std::unordered_map<std::string, ObjectPtr>;
  struct StoreData {
    ObjectPtr value;
    bool found;
  };
  explicit EnvironmentImpl();
  explicit EnvironmentImpl(std::shared_ptr<EnvironmentImpl> outer);
  ~EnvironmentImpl() = default;
  StoreData get(const std::string& name);
  ObjectPtr set(const std::string& name, ObjectPtr value);
  std::unordered_map<std::string, ObjectPtr> store_;
  std::shared_ptr<EnvironmentImpl> outer_;
};

using Environment = std::shared_ptr<EnvironmentImpl>;

class Function : public Object {
public:
  Function(parser::ast::Parameters params, std::unique_ptr<parser::ast::BlockStatement> body, Environment env);
  ~Function() override = default;
  std::string to_string() const override;
  std::string type() const override;
  parser::ast::Parameters parameters;
  std::unique_ptr<parser::ast::BlockStatement> body;
  Environment env_;
};

Environment new_enclosed_environment(Environment outer);
} // namespace evaluator
} // namespace monkey
