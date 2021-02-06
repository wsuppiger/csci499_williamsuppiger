// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value.h"

namespace backend {

void KeyValue::Put(const std::string& key, const std::string& value) {
  std::unordered_map<std::string, std::vector<std::string>>::iterator
      storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    storage_iter->second.push_back(value);
  } else {
    std::vector<std::string> value_list {value};
    storage_[key] = value_list;
  }
}

std::vector<std::string> KeyValue::Get(const std::string& key) const {
  std::unordered_map<std::string, std::vector<std::string>>::const_iterator
      storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    return storage_iter->second;
  }
  return {};
}

void KeyValue::Remove(const std::string& remove) {
  storage_.erase(remove);
}

}  // namespace backend
