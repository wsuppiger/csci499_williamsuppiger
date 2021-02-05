// Copyright (c) 2021, USC
// All rights reserved.
#ifndef SRC_KEY_VALUE_H_
#define SRC_KEY_VALUE_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace backend {

// TODO: CHANGE BYTES TYPE
typedef std::string BYTES;

// key value storage for backend application
class KeyValue {
 public:
  KeyValue() {}

  virtual ~KeyValue() {}

  void put(const BYTES& key, const BYTES& value);
  void get(const BYTES& key) const;
  void remove(const BYTES& remove);

 private:
  // stores key value pairs in map of vectors
  std::unordered_map<BYTES, std::vector<BYTES>> storage_;
};
}  // namespace backend

#endif  // SRC_KEY_VALUE_H_
