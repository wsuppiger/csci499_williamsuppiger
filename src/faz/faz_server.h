// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_FAZ_FAZ_SERVER_H_
#define SRC_FAZ_FAZ_SERVER_H_

#include <grpcpp/grpcpp.h>

#include "../caw/caw_function.h"
#include "../key_value/key_value_client.h"
#include "faz.grpc.pb.h"

namespace csci499 {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using google::protobuf::Any;

using faz::EventReply;
using faz::EventRequest;
using faz::FazService;
using faz::HookReply;
using faz::HookRequest;
using faz::UnhookReply;
using faz::UnhookRequest;

class FazServer final : public FazService::Service {
 public:
  FazServer()
      : kv_(grpc::CreateChannel("0.0.0.0:50001",
                                grpc::InsecureChannelCredentials())) {}

  // hook function found in the caw functions class
  Status hook(ServerContext* context, const HookRequest* request,
              HookReply* reply) override;

  // unhook function found in the caw functions class
  Status unhook(ServerContext* context, const UnhookRequest* request,
                UnhookReply* reply) override;

  // executes event found in caw functions
  Status event(ServerContext* context, const EventRequest* request,
               EventReply* reply) override;

 private:
  // key value client used to connect to kv server
  KeyValueClient kv_;
};
}  // namespace csci499
#endif  // SRC_FAZ_FAZ_SERVER_H_
