// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_FAZ_FAZ_CLIENT_H_
#define SRC_FAZ_FAZ_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "caw.grpc.pb.h"
#include "faz.grpc.pb.h"

namespace csci499 {

using faz::FazService;

using google::protobuf::Any;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::StatusCode;
using grpc::ClientReader;

using faz::EventReply;
using faz::EventRequest;
using faz::FazService;
using faz::HookReply;
using faz::HookRequest;
using faz::UnhookReply;
using faz::UnhookRequest;

using caw::Caw;
using caw::CawReply;
using caw::CawRequest;
using caw::FollowReply;
using caw::FollowRequest;
using caw::ProfileReply;
using caw::ProfileRequest;
using caw::ReadReply;
using caw::ReadRequest;
using caw::RegisteruserReply;
using caw::RegisteruserRequest;
using caw::Timestamp;
using caw::StreamRequest;
using caw::StreamReply;

// faz client implementaion for interacting with faz server
class FazClient {
 public:
  explicit FazClient(std::shared_ptr<Channel> channel)
      : stub_(FazService::NewStub(channel)) {}

  virtual ~FazClient() {}

  // hook function on faz server
  bool Hook(int event_type, const std::string& event_function);

  // unhook a function on faz server
  bool Unhook(int event_type);

  // send an event request to faz server and return proto Reply object
  Status Event(int event_type, Any& payload, EventReply& reply);

  // Reads any messages posted to a registered stream 
  // And passes them into a callback function to print 
  // Information to the user
  Status Stream(int event_type, Any& payload,
                std::function<void(EventReply)>& print_caw);
 private:
  // faz storage object
  std::unique_ptr<FazService::Stub> stub_;
};

}  // namespace csci499
#endif  // SRC_FAZ_FAZ_CLIENT_H_
