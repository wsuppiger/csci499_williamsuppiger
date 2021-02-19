// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_CAW_CAW_FUNCTION_H_
#define SRC_CAW_CAW_FUNCTION_H_

#include <grpcpp/grpcpp.h>

#include <string>
#include <unordered_map>

#include "../key_value/key_value_interface.h"
#include "caw.grpc.pb.h"

namespace csci499 {

using google::protobuf::Any;
using grpc::Status;
using grpc::StatusCode;

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

// struct for caw function reply message
struct CawFuncReply {
  Status status;
  std::string message;
};

class CawFunction {
 public:
  // registers user to new account
  static CawFuncReply RegisterUser(const Any& payload, KeyValueInterface& kv);

  // creates new caw
  static CawFuncReply Caw(const Any& payload, KeyValueInterface& kv);

  // follows another user
  static CawFuncReply Follow(const Any& payload, KeyValueInterface& kv);

  // reads caw
  static CawFuncReply Read(const Any& payload, KeyValueInterface& kv);

  // shows following and followers for user
  static CawFuncReply Profile(const Any& payload, KeyValueInterface& kv);

  // map names of functions to functions
  static std::unordered_map<
      std::string, std::function<CawFuncReply(const Any&, KeyValueInterface&)> >
      function_map_;

 private:
  // check if user exists
  static bool UserExists(const std::string& username, KeyValueInterface& kv);
};
}  // namespace csci499
#endif  // SRC_CAW_CAW_FUNCTION_H_
