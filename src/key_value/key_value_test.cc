// Copyright (c) 2021, USC
// All rights reserved.
#include "key_value.h"

#include <gtest/gtest.h>

#include <vector>

namespace backend {

// test initialization of backend
TEST(KeyValue, OneVectOneValue) {
  KeyValue test;
  test.Put("a", "0");
  EXPECT_EQ(test.Get("a")[0], "0");
}

TEST(KeyValue, OneVectTwoValues) {
  KeyValue test;
  test.Put("a", "0");
  test.Put("a", "1");
  EXPECT_EQ(test.Get("a")[0], "0");
  EXPECT_EQ(test.Get("a")[1], "1");
}

TEST(KeyValue, TwoVectTwoTwoValues) {
  KeyValue test;
  test.Put("a", "0");
  test.Put("a", "1");
  test.Put("b", "2");
  test.Put("b", "3");
  EXPECT_EQ(test.Get("a")[0], "0");
  EXPECT_EQ(test.Get("a")[1], "1");
  EXPECT_EQ(test.Get("b")[0], "2");
  EXPECT_EQ(test.Get("b")[1], "3");
}

TEST(KeyValue, RemoveEmpty) {
  KeyValue test;
  std::vector<std::string> empty;
  test.Remove("a");
}

TEST(KeyValue, RemoveOneElement) {
  KeyValue test;
  test.Put("a", "0");
  test.Remove("a");
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

TEST(KeyValue, RemoveTwoElement) {
  KeyValue test;
  test.Put("a", "0");
  test.Put("a", "1");
  test.Remove("a");
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

TEST(KeyValue, Empty) {
  KeyValue test;
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

}  // namespace backend
