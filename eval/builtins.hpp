#pragma once

#include "object.hpp"
#include <iostream>

namespace monkey::evaluator {
using Builtins = std::unordered_map<std::string, std::shared_ptr<Builtin>>;

inline Builtins create_builtins() {

  static Builtins builtins;
  const static auto len = [](const Results &args) -> ObjectPtr {
    if (args.size() != 1) {
      return makeError("wrong number of arguments. want=1, got=", args.size());
    }
    if (args[0]->type() == STRING_OBJ) {
      auto str = std::dynamic_pointer_cast<String>(args[0]);
      return std::make_shared<Integer>(str->value_.size());
    }
    //    if (args[0]->type() == ARRAY_OBJ) {
    //      auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    //      return std::make_shared<Integer>(arr->elements_.size());
    //    }
    return makeError("argument to `len` not supported, got", args[0]->type());
  };

  builtins.insert({"len", std::make_shared<Builtin>(len)});
  return builtins;
}

} // namespace monkey::evaluator
