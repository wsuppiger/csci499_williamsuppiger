// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value.h"

namespace backend {

void KeyValue::Put(const std::string& key, const std::string& value) {
  lock_.lock();
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    storage_iter->second.push_back(value);
  } else {
    std::vector<std::string> value_list{value};
    storage_[key] = value_list;
  }
  lock_.unlock();
}

std::vector<std::string> KeyValue::Get(const std::string& key) {
  lock_.lock();
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    lock_.unlock();
    return storage_iter->second;
  }
  lock_.unlock();
  return {};
}

void KeyValue::Remove(const std::string& remove) {
  lock_.lock();
  storage_.erase(remove);
  lock_.unlock();
}

}  // namespace backend
