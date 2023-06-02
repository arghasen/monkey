#pragma once

namespace monkey {
namespace evaluator {

using OBJECT_TYPE = const char *;
constexpr OBJECT_TYPE INTEGER_OBJ = "INTEGER";
constexpr OBJECT_TYPE BOOLEAN_OBJ = "BOOLEAN";
constexpr OBJECT_TYPE NULL_OBJ = "NULL";

class Object{
public:
    virtual ~Object() = default;
    virtual std::string to_string() const = 0;
    virtual std::string type() const = 0;
};

class Integer : public Object{
public:
    explicit Integer(int value);
    ~Integer() override = default;
    std::string to_string() const override;
    std::string type() const override;
    int value_;
};

class Boolean : public Object{
public:
    explicit Boolean(bool value);
    ~Boolean() override = default;
    std::string to_string() const override;
    std::string type() const override;
    bool value_;
};

class Null : public Object{
public:
    Null() = default;
    ~Null() override = default;
    std::string to_string() const override;
    std::string type() const override;
};

} // namespace evaluator
} // namespace monkey