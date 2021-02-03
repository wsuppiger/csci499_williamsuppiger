// Copyright (c) 2021, USC
// All rights reserved.

#include "keyValue.h"

#include <gtest/gtest.h>

namespace backend {
// test initialization of backend
TEST(CreateKeyValue, Initialization) {
  KeyValue backend;
  backend.put('a', 'b');
}

}  // namespace backend
