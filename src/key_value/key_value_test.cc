// Copyright (c) 2021, USC
// All rights reserved.
#include "key_value.h"

#include <gtest/gtest.h>

#include <vector>

namespace {

using csci499::KeyValue;
using kvstore::KeyValueSnapshot;

// insert of one value for one key
TEST(KeyValue, OneVectOneValue) {
  KeyValue test;
  test.Put("a", "0");
  EXPECT_EQ(test.Get("a")[0], "0");
}

// insert of two values for one key
TEST(KeyValue, OneVectTwoValues) {
  KeyValue test;
  test.Put("a", "0");
  test.Put("a", "1");
  EXPECT_EQ(test.Get("a")[0], "0");
  EXPECT_EQ(test.Get("a")[1], "1");
}

// insert of two values each for two keys
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

// remove an empty key
TEST(KeyValue, RemoveEmpty) {
  KeyValue test;
  std::vector<std::string> empty;
  test.Remove("a");
  EXPECT_EQ(test.Get("a"), empty);
}

// remove one element
TEST(KeyValue, RemoveOneElement) {
  KeyValue test;
  test.Put("a", "0");
  test.Remove("a");
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

// remove two elements from key
TEST(KeyValue, RemoveTwoElement) {
  KeyValue test;
  test.Put("a", "0");
  test.Put("a", "1");
  test.Remove("a");
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

// return empty for no insertions
TEST(KeyValue, Empty) {
  KeyValue test;
  std::vector<std::string> empty;
  EXPECT_EQ(test.Get("a"), empty);
}

// Snapshot with one key and one value
TEST(KeyValue, SnapshotOneKeyOneValue) {
  KeyValue test;
  test.Put("a", "1");
  KeyValueSnapshot snapshot;
  test.CreateSnapshot(snapshot);
  KeyValue replica;
  replica.LoadSnapshot(snapshot);
  EXPECT_EQ(replica.Get("a")[0], "1");
}

// Snapshot with one key and two values
TEST(KeyValue, SnapshotOneKeyTwoValues) {
  KeyValue test;
  test.Put("a", "1");
  test.Put("a", "2");
  KeyValueSnapshot snapshot;
  test.CreateSnapshot(snapshot);
  KeyValue replica;
  replica.LoadSnapshot(snapshot);
  EXPECT_EQ(replica.Get("a")[0], "1");
  EXPECT_EQ(replica.Get("a")[1], "2");
}

// Snapshot with two keys and one value
TEST(KeyValue, SnapshotTwoKeysOneValue) {
  KeyValue test;
  test.Put("a", "1");
  test.Put("b", "2");
  KeyValueSnapshot snapshot;
  test.CreateSnapshot(snapshot);
  KeyValue replica;
  replica.LoadSnapshot(snapshot);
  EXPECT_EQ(replica.Get("a")[0], "1");
  EXPECT_EQ(replica.Get("b")[0], "2");
}
}  // namespace
