// Copyright (c) 2021, USC
// All rights reserved.

#ifndef SRC_CAW_CAW_FUNCTION_H_
#define SRC_CAW_CAW_FUNCTION_H_

#include <grpcpp/grpcpp.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../key_value/key_value_interface.h"
#include "caw.grpc.pb.h"

namespace csci499 {

using google::protobuf::Any;
using grpc::Status;
using grpc::StatusCode;

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
  static CawFuncReply CawCreate(const Any& payload, KeyValueInterface& kv);

  // follows another user
  static CawFuncReply Follow(const Any& payload, KeyValueInterface& kv);

  // reads caw
  static CawFuncReply Read(const Any& payload, KeyValueInterface& kv);

  // shows following and followers for user
  static CawFuncReply Profile(const Any& payload, KeyValueInterface& kv);

  // This function will parse the payload and look for any hashtags 
  // It will then call the callback functions for that corresponding 
  // hashtag without ever interracting with the kvstore
  static CawFuncReply Stream(const Any& payload,
                              std::unordered_map<std::string, 
                              std::vector<std::function<bool(const Any&)>> >&
                                current_streamers_);

  // Returns a vector containing all hashtags in some text.
  static std::vector<std::string> GetHashtags(const std::string& message);

  // map names of functions to functions
  static std::unordered_map<
      std::string, std::function<CawFuncReply(const Any&, KeyValueInterface&)> >
      function_map_;

  // map names of stream_functions to functions
  // Seperate because stream_function declaration is different
  // TODO: Consider using a typedef
  static std::unordered_map<
      std::string, std::function<CawFuncReply(const Any&,
                                              std::unordered_map<std::string, 
                                                std::vector<std::function<
                                                  bool(const Any&)>> >&)
                                > > stream_function_map_;

 private:
  // check if user exists
  static bool UserExists(const std::string& username, KeyValueInterface& kv);
  // DFS recursively search for all child Caws and add to caws vector
  static void ReadReplys(const std::string& caw_id, KeyValueInterface& kv,
                         std::vector<Caw>& caws);
  // Lock enables thread safe operation
  // during the 'Stream' function. Becuase 
  // that access shared memory (current_streamers_)
  static std::mutex lock_;
};
}  // namespace csci499
#endif  // SRC_CAW_CAW_FUNCTION_H_
