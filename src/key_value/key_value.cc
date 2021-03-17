// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value.h"

#include <glog/logging.h>

namespace csci499 {

void KeyValue::Put(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lock(lock_);
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    storage_iter->second.push_back(value);
    LOG(INFO) << "appended value to list for key";
  } else {
    std::vector<std::string> value_list{value};
    storage_[key] = value_list;
    LOG(INFO) << "stored new key value pair";
  }
}

std::vector<std::string> KeyValue::Get(const std::string& key) {
  std::lock_guard<std::mutex> lock(lock_);
  auto storage_iter = storage_.find(key);
  if (storage_iter != storage_.end()) {
    LOG(INFO) << "found value(s) for key " << key;
    return storage_iter->second;
  }
  LOG(WARNING) << "could not find values for key  " << key;
  return {};
}

void KeyValue::Remove(const std::string& key) {
  std::lock_guard<std::mutex> lock(lock_);
  storage_.erase(key);
  LOG(INFO) << "removed any existing values for key " << key;
}

void KeyValue::CreateSnapshot(KeyValueSnapshot& snapshot) {
  std::lock_guard<std::mutex> lock(lock_);
  for (const auto& kv : storage_) {  // loop over eaach key value(s) pair
    KeyValuePair pair;
    pair.set_key(kv.first);
    std::vector<std::string> values = kv.second;
    *pair.mutable_values() = {values.begin(), values.end()};
    KeyValuePair* p = snapshot.add_pairs();
    p->CopyFrom(pair);
  }
  LOG(INFO) << "Took Snapshot of current kv state";
}

void KeyValue::LoadSnapshot(const KeyValueSnapshot& snapshot) {
  std::lock_guard<std::mutex> lock(lock_);
  storage_.clear();
  for (const auto& pair : snapshot.pairs()) {
    const std::string key = pair.key();
    std::vector<std::string> values(pair.values().begin(), pair.values().end());
    storage_.insert({key, values});
    LOG(INFO) << "Loaded Snapshot key: " << key;
  }
  LOG(INFO) << "Completed loading Snapshot";
}

}  // namespace csci499
