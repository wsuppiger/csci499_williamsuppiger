// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_KEY_VALUE_KEY_VALUE_CLIENT_H_
#define SRC_KEY_VALUE_KEY_VALUE_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <memory>
#include <string>
#include <vector>

#include "key_value.grpc.pb.h"
#include "key_value_interface.h"

namespace csci499 {
using grpc::Channel;

using kvstore::KeyValueStore;

// key value client implementtaion for csci499
class KeyValueClient : public KeyValueInterface {
 public:
  explicit KeyValueClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  virtual ~KeyValueClient() {}

  // package and send rpc call put to server
  virtual void Put(const std::string& key, const std::string& value);

  // package and retrieve rpc call get from server
  virtual std::vector<std::string> Get(const std::string& key);

  // rpc remove key and all values on server
  virtual void Remove(const std::string& key);

 private:
  // key value storage object
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

}  // namespace csci499
#endif  // SRC_KEY_VALUE_KEY_VALUE_CLIENT_H_
