// Copyright (c) 2021, USC
// All rights reserved.
#ifndef SRC_KEY_VALUE_H_
#define SRC_KEY_VALUE_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace backend {

// key value storage for backend application
class KeyValue {
 public:
  KeyValue(): storage_() {}

  virtual ~KeyValue() {}

  void Put(const std::string& key, const std::string& value);
  void Get(const std::string& key) const;
  void Remove(const std::string& remove);

 private:
  // stores key value pairs in map of vectors
  std::unordered_map<std::string, std::vector<std::string>> storage_;
};
}  // namespace backend

#endif  // SRC_KEY_VALUE_H_
