// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value.h"

#include <glog/logging.h>

namespace csci499 {

void KeyValue::Put(const std::string& key, const std::string& value) {
  lock_.lock();
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    storage_iter->second.push_back(value);
    LOG(INFO) << "appended value to list for key";
  } else {
    std::vector<std::string> value_list{value};
    storage_[key] = value_list;
    LOG(INFO) << "stored new key value pair";
  }
  lock_.unlock();
}

std::vector<std::string> KeyValue::Get(const std::string& key) {
  lock_.lock();
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    lock_.unlock();
    LOG(INFO) << "found value(s) for key " << key;
    return storage_iter->second;
  }
  lock_.unlock();
  LOG(WARNING) << "could not find values for key  " << key;
  return {};
}

void KeyValue::Remove(const std::string& key) {
  lock_.lock();
  storage_.erase(key);
  lock_.unlock();
  LOG(INFO) << "removed any existing values for key " << key;
}

}  // namespace csci499
