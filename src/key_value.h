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

class KeyValue {
 public:
  KeyValue() {}

  ~KeyValue() {}

  void put(const BYTES& key, const BYTES& value);
  void get(const BYTES& key) const;
  void remove(const BYTES& remove);

 private:
  std::unordered_map<BYTES, std::vector<BYTES>> storage_;
};
}  // namespace backend

#endif  // SRC_KEY_VALUE_H_
