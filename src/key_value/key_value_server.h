// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_KEY_VALUE_KEY_VALUE_SERVER_H_
#define SRC_KEY_VALUE_KEY_VALUE_SERVER_H_

#include <grpcpp/grpcpp.h>

#include "key_value.grpc.pb.h"
#include "key_value.h"

namespace csci499 {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

// key value server implementtaion for csci499
class KeyValueServer final : public KeyValueStore::Service {
 public:
  KeyValueServer() : storage_() {}

  // grpc put call
  Status put(ServerContext* context, const PutRequest* request,
             PutReply* reply) override;

  // grpc get call
  Status get(ServerContext* context,
             ServerReaderWriter<GetReply, GetRequest>* stream) override;

  // grpc remove call
  Status remove(ServerContext* context, const RemoveRequest* request,
                RemoveReply* reply) override;

 private:
  // key value storage object
  KeyValue storage_;
  // lock for key value storeage object
};

}  // namespace csci499
#endif  // SRC_KEY_VALUE_KEY_VALUE_SERVER_H_
