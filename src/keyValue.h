// Copyright (c) 2021, USC
// All rights reserved.
#ifndef SRC_KEYVALUE_H_
#define SRC_KEYVALUE_H_
#include <unordered_map>
#include <vector>

namespace backend {

// TODO: CHANGE BYTES TYPE
typedef char BYTES;

class KeyValue {
 public:
  KeyValue() {}

  ~KeyValue() {}

  void put(const BYTES& key, const BYTES& value);
  void get(const BYTES& key) const;
  void remove(const BYTES& remove);

 private:
  std::unordered_map<BYTES, std::vector<BYTES>> storage_;
}
}  // namespace backend

#endif  // SRC_KEYVALUE_H_
