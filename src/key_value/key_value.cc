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

void KeyValue::TakeSnapshot(KeyValueSnapshot& snapshot) {
  lock_.lock();
  for (auto kv : storage_) {  // loop over eaach key value(s) pair
    KeyValuePair pair;
    pair.set_key(kv.first);
    std::vector<std::string> values = kv.second;
    *pair.mutable_values() = {values.begin(), values.end()};
    KeyValuePair* p = snapshot.add_pairs();
    p->CopyFrom(pair);
  }
  LOG(INFO) << "Took Snapshot of current kv state";
  lock_.unlock();
}

void KeyValue::LoadSnapshot(KeyValueSnapshot& snapshot) {
  lock_.lock();
  storage_.clear();
  for (auto pair : snapshot.pairs()) {
    std::string key = pair.key();
    std::vector<std::string> values(pair.values().begin(), pair.values().end());
    storage_.insert({key, values});
    LOG(INFO) << "Loaded Snapshot key: " << key;
  }
  LOG(INFO) << "Completed loading Snapshot";
  lock_.unlock();
}

}  // namespace csci499
