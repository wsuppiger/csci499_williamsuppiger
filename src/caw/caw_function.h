// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_CAW_CAW_FUNCTION_H_
#define SRC_CAW_CAW_FUNCTION_H_

#include <grpcpp/grpcpp.h>

#include <string>
#include <unordered_map>

#include "../key_value/key_value_client.h"
#include "caw.grpc.pb.h"

namespace csci499 {

using google::protobuf::Any;
using grpc::Status;
using grpc::StatusCode;

using caw::RegisteruserRequest;
using caw::RegisteruserReply;
using caw::CawRequest;
using caw::CawReply;
using caw::FollowRequest;
using caw::FollowReply;
using caw::ReadRequest;
using caw::ReadReply;
using caw::ProfileRequest;
using caw::ProfileReply;

// struct for caw function reply message
struct CawFuncReply {
  Status status;
  std::string message;
};

class CawFunction {
  typedef CawFuncReply (CawFunction::*caw_method_t)(const Any& payload);
  typedef std::unordered_map<std::string, caw_method_t> caw_func_map_t;

 public:
  explicit CawFunction(KeyValueClient& kv)
      : kv_(kv),
        function_map_{{"registeruser", &CawFunction::RegisterUser}}

  {}

  // name of function to function pointer map
  caw_method_t GetFunction(const std::string);

  // register user to kv storage
  CawFuncReply RegisterUser(const Any& payload);

 private:
  // client to communicate with key value server
  KeyValueClient& kv_;
  // map names of functions to functions
  caw_func_map_t function_map_;
};
}  // namespace csci499
#endif  // SRC_CAW_CAW_FUNCTION_H_
