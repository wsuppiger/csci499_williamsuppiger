// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_KEY_VALUE_KEY_VALUE_SERVER_H_
#define SRC_KEY_VALUE_KEY_VALUE_SERVER_H_

#include <grpcpp/grpcpp.h>

#include <string>

#include "key_value.grpc.pb.h"
#include "key_value.h"
#include "kv_persist.grpc.pb.h"

namespace csci499 {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValuePair;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;
using kvstore::KeyValueSnapshot;

// key value server implementtaion for csci499
class KeyValueServer final : public KeyValueStore::Service {
 public:
  // default constructor
  KeyValueServer();

  // constructor for persistent file storage
  KeyValueServer(std::string filename);

  // grpc put call and store data in storage file if persistent kv
  Status put(ServerContext* context, const PutRequest* request,
             PutReply* reply) override;

  // grpc get call
  Status get(ServerContext* context,
             ServerReaderWriter<GetReply, GetRequest>* stream) override;

  // grpc remove call and remove all data in storage file if persistent kv
  Status remove(ServerContext* context, const RemoveRequest* request,
                RemoveReply* reply) override;

 private:
  // stores snapshot of current KeyValue Object in provided file
  bool StoreSnapshot(const std::string filename);

  // key value storage object
  KeyValue storage_;
  // file for persistent storage (empty string if no persistent)
  std::string storage_file_;
};

}  // namespace csci499
#endif  // SRC_KEY_VALUE_KEY_VALUE_SERVER_H_
