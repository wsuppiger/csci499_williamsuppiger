// Copyright (c) 2021, USC
// All rights reserved.
#include "caw_function.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "../key_value/key_value.h"
#include "../key_value/key_value_server.h"

namespace {

using google::protobuf::Any;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using csci499::CawFuncReply;
using csci499::CawFunction;
using csci499::KeyValue;

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

// for registering user during later tests
void createUser(KeyValue& kv, const std::string& username) {
  auto caw_method = CawFunction::function_map_["registeruser"];
  RegisteruserRequest request;
  request.set_username(username);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
}

// test register one user
TEST(RegisterUser, One) {
  KeyValue kv;
  auto caw_method = CawFunction::function_map_["registeruser"];
  RegisteruserRequest request;
  request.set_username("test1");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_EQ(reply.message, "");
  ASSERT_TRUE(reply.status.ok());
}

// test register two users
TEST(RegisterUser, Two) {
  KeyValue kv;
  auto caw_method = CawFunction::function_map_["registeruser"];
  RegisteruserRequest request;
  request.set_username("test1");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_EQ(reply.message, "");
  ASSERT_TRUE(reply.status.ok());
}

// test register user empty
TEST(RegisterUser, empty) {
  KeyValue kv;
  auto caw_method = CawFunction::function_map_["registeruser"];
  RegisteruserRequest request;
  request.set_username("");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// test register user duplicate
TEST(RegisterUser, duplicate) {
  KeyValue kv;
  auto caw_method = CawFunction::function_map_["registeruser"];
  // first request
  RegisteruserRequest request1;
  request1.set_username("username");
  Any any1 = Any();
  any1.PackFrom(request1);
  CawFuncReply reply1 = caw_method(any1, kv);
  ASSERT_TRUE(reply1.status.ok());
  // second request
  RegisteruserRequest request2;
  request2.set_username("username");
  Any any2 = Any();
  any2.PackFrom(request2);
  CawFuncReply reply2 = caw_method(any2, kv);
  ASSERT_FALSE(reply2.status.ok());
}

// test following a user who does not exist
TEST(Follow, NotExists) {
  KeyValue kv;
  auto caw_method = CawFunction::function_map_["follow"];
  FollowRequest request;
  request.set_username("user");
  request.set_to_follow("non-exist");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// test a user following themself
TEST(Follow, Themself) {
  KeyValue kv;
  createUser(kv, "user");
  auto caw_method = CawFunction::function_map_["follow"];
  FollowRequest request;
  request.set_username("user");
  request.set_to_follow("user");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// test one to one following
TEST(FollowAndProfile, OneToOne) {
  KeyValue kv;
  createUser(kv, "main-user");
  createUser(kv, "other-user");

  auto caw_method = CawFunction::function_map_["follow"];  // setup follow
  FollowRequest request;
  request.set_username("main-user");
  request.set_to_follow("other-user");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());

  ProfileRequest main_request;  // check profile to assert following
  caw_method = CawFunction::function_map_["profile"];
  main_request.set_username("main-user");
  any.PackFrom(main_request);
  CawFuncReply main_reply = caw_method(any, kv);
  ProfileReply main_profile;
  main_profile.ParseFromString(main_reply.message);
  ASSERT_TRUE(main_reply.status.ok());
  ASSERT_EQ(main_profile.following()[0], "other-user");
  ASSERT_EQ(main_profile.followers().size(), 0);

  ProfileRequest other_request;  // check profile to assert follower
  other_request.set_username("other-user");
  any.PackFrom(other_request);
  CawFuncReply other_reply = caw_method(any, kv);
  ProfileReply other_profile;
  other_profile.ParseFromString(other_reply.message);
  ASSERT_TRUE(other_reply.status.ok());
  ASSERT_EQ(other_profile.followers()[0], "main-user");
  ASSERT_EQ(other_profile.following().size(), 0);
}

// test profile not existing
TEST(Profile, NotExists) {
  KeyValue kv;
  ProfileRequest request;
  auto caw_method = CawFunction::function_map_["profile"];
  request.set_username("main-user");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

}  // namespace
