// Copyright (c) 2021, USC
// All rights reserved.

#include "caw_function.h"

namespace csci499 {

CawFunction::caw_method_t CawFunction::GetFunction(
    const std::string function_name) {
  auto method = function_map_.find(function_name);
  return (method != function_map_.end()) ? method->second : nullptr;
}

CawFuncReply CawFunction::RegisterUser(const Any& payload) {
  // implementation
  return {Status::OK, "test"};
}

}  // namespace csci499
