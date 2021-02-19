// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_KEY_VALUE_KEY_VALUE_INTERFACE_H_
#define SRC_KEY_VALUE_KEY_VALUE_INTERFACE_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace csci499 {

// key value storage for csci499 application
class KeyValueInterface {
 public:
  virtual ~KeyValueInterface() {}

  // store item
  virtual void Put(const std::string& key, const std::string& value) = 0;

  // return stored items
  virtual std::vector<std::string> Get(const std::string& key) = 0;

  // remove key and associated values
  virtual void Remove(const std::string& key) = 0;
};
}  // namespace csci499

#endif  // SRC_KEY_VALUE_KEY_VALUE_INTERFACE_H_
