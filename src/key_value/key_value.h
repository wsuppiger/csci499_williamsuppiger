// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_KEY_VALUE_KEY_VALUE_H_
#define SRC_KEY_VALUE_KEY_VALUE_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace csci499 {

// key value storage for csci499 application
class KeyValue {
 public:
  KeyValue() : storage_(), lock_() {}

  virtual ~KeyValue() {}

  void Put(const std::string& key, const std::string& value);
  std::vector<std::string> Get(const std::string& key);
  void Remove(const std::string& key);

 private:
  // stores key value pairs in map of vectors
  std::unordered_map<std::string, std::vector<std::string>> storage_;
  // lock to prevent race conditions on storage access
  std::mutex lock_;
};
}  // namespace csci499

#endif  // SRC_KEY_VALUE_KEY_VALUE_H_
