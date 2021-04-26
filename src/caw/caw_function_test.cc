// Copyright (c) 2021, USC
// All rights reserved.
#include "caw_function.h"

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <unistd.h>

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

// for registering user quickly in tests
void CreateUser(KeyValue& kv, const std::string& username) {
  auto caw_method = CawFunction::function_map_["registeruser"];
  RegisteruserRequest request;
  request.set_username(username);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
}

// for registering user quickly in tests and returns caw id
std::string CreateCaw(KeyValue& kv, const std::string& username,
                      const std::string& text, const std::string& parent_id) {
  auto caw_method = CawFunction::function_map_["caw"];
  CawRequest request;
  request.set_username(username);
  request.set_text(text);
  request.set_parent_id(parent_id);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  CawReply caw_reply;
  caw_reply.ParseFromString(reply.message);
  Caw caw = caw_reply.caw();
  return caw.id();
}

// register one user
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

// register two users
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

// register user empty
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

// register user duplicate
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

// following a user who does not exist
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

// a user following themself
TEST(Follow, Themself) {
  KeyValue kv;
  CreateUser(kv, "user");
  auto caw_method = CawFunction::function_map_["follow"];
  FollowRequest request;
  request.set_username("user");
  request.set_to_follow("user");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// one to one following
TEST(FollowAndProfile, OneToOne) {
  KeyValue kv;
  CreateUser(kv, "main-user");
  CreateUser(kv, "other-user");

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

// profile not existing
TEST(Profile, NotExists) {
  KeyValue kv;
  ProfileRequest request;
  auto caw_method = CawFunction::function_map_["profile"];
  request.set_username("user");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// create Caw with invalid user
TEST(CawCreate, UserNotExists) {
  KeyValue kv;

  CawRequest request;
  auto caw_method = CawFunction::function_map_["caw"];
  request.set_username("empty");
  request.set_text("sample text");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// create caw with bad parent id
TEST(CawCreate, BadParentId) {
  KeyValue kv;
  CreateUser(kv, "user1");

  CawRequest request;
  auto caw_method = CawFunction::function_map_["caw"];
  request.set_username("user1");
  request.set_text("sample text");
  request.set_parent_id("BadParentId");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// create caw with no parent
TEST(CawCreate, NoParent) {
  KeyValue kv;
  CreateUser(kv, "user1");

  CawRequest request;
  auto caw_method = CawFunction::function_map_["caw"];
  request.set_username("user1");
  request.set_text("sample text");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());
  CawReply caw_reply;
  caw_reply.ParseFromString(reply.message);
  Caw caw = caw_reply.caw();
  // verify all parts of Caw
  ASSERT_EQ(caw.username(), "user1");
  ASSERT_EQ(caw.text(), "sample text");
  ASSERT_EQ(caw.id(), "0");
  ASSERT_EQ(caw.parent_id(), "");
  Timestamp time = caw.timestamp();
  timeval curr_time;
  gettimeofday(&curr_time, NULL);
  EXPECT_TRUE(curr_time.tv_sec - time.seconds() < 1 &&
              curr_time.tv_sec - time.seconds() >= 0);
  EXPECT_TRUE(curr_time.tv_usec - time.useconds() < 100000 &&
              curr_time.tv_usec - time.useconds() >= 0);
}

// create caw with parent
TEST(CawCreate, Parent) {
  KeyValue kv;
  CreateUser(kv, "user1");
  CreateCaw(kv, "user1", "sample text", "");

  CawRequest request;
  auto caw_method = CawFunction::function_map_["caw"];
  request.set_username("user1");
  request.set_text("sample text");
  request.set_parent_id("0");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());
  CawReply caw_reply;
  caw_reply.ParseFromString(reply.message);
  Caw caw = caw_reply.caw();
  // verify all parts of Caw
  ASSERT_EQ(caw.username(), "user1");
  ASSERT_EQ(caw.text(), "sample text");
  ASSERT_EQ(caw.id(), "1");
  ASSERT_EQ(caw.parent_id(), "0");
  Timestamp time = caw.timestamp();
  timeval curr_time;
  gettimeofday(&curr_time, NULL);
  EXPECT_TRUE(curr_time.tv_sec - time.seconds() < 1 &&
              curr_time.tv_sec - time.seconds() >= 0);
  EXPECT_TRUE(curr_time.tv_usec - time.useconds() < 100000 &&
              curr_time.tv_usec - time.useconds() >= 0);
}

// read caw that does not exist
TEST(Read, NotExists) {
  KeyValue kv;

  ReadRequest request;
  auto caw_method = CawFunction::function_map_["read"];
  request.set_caw_id("BadId");
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_FALSE(reply.status.ok());
}

// read one caw
TEST(Read, One) {
  KeyValue kv;
  CreateUser(kv, "user1");
  std::string caw_id = CreateCaw(kv, "user1", "sample text", "");

  ReadRequest request;
  auto caw_method = CawFunction::function_map_["read"];
  request.set_caw_id(caw_id);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());
  ReadReply read_reply;
  read_reply.ParseFromString(reply.message);
  Caw caw = read_reply.caws()[0];
  ASSERT_EQ(caw.username(), "user1");
  ASSERT_EQ(caw.text(), "sample text");
  ASSERT_EQ(caw.id(), caw_id);
  Timestamp time = caw.timestamp();
  timeval curr_time;
  gettimeofday(&curr_time, NULL);
  EXPECT_TRUE(curr_time.tv_sec - time.seconds() < 1 &&
              curr_time.tv_sec - time.seconds() >= 0);
  EXPECT_TRUE(curr_time.tv_usec - time.useconds() < 100000 &&
              curr_time.tv_usec - time.useconds() >= 0);
}

// caw reply feature with two caws (parent child)
TEST(Read, ParentChild) {
  KeyValue kv;
  CreateUser(kv, "user1");
  std::string caw_parent_id = CreateCaw(kv, "user1", "parent", "");
  std::string caw_child_id =
      CreateCaw(kv, "user1", "child reply", caw_parent_id);

  ReadRequest request;
  auto caw_method = CawFunction::function_map_["read"];
  request.set_caw_id(caw_parent_id);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());
  ReadReply read_reply;
  read_reply.ParseFromString(reply.message);
  ASSERT_EQ(read_reply.caws()[0].id(), caw_parent_id);
  ASSERT_EQ(read_reply.caws()[1].id(), caw_child_id);
}

// caw reply feature with two reply caws
TEST(Read, TwoReply) {
  KeyValue kv;
  CreateUser(kv, "user1");
  std::string caw_parent_id = CreateCaw(kv, "user1", "parent", "");
  std::string caw_child_id1 =
      CreateCaw(kv, "user1", "child reply 1", caw_parent_id);
  std::string caw_child_id2 =
      CreateCaw(kv, "user1", "child reply 2", caw_parent_id);

  ReadRequest request;
  auto caw_method = CawFunction::function_map_["read"];
  request.set_caw_id(caw_parent_id);
  Any any = Any();
  any.PackFrom(request);
  CawFuncReply reply = caw_method(any, kv);
  ASSERT_TRUE(reply.status.ok());
  ReadReply read_reply;
  read_reply.ParseFromString(reply.message);
  ASSERT_EQ(read_reply.caws()[0].id(), caw_parent_id);
  ASSERT_EQ(read_reply.caws()[1].id(), caw_child_id1);
  ASSERT_EQ(read_reply.caws()[2].id(), caw_child_id2);
}

// Simple test to see if Hash function parses correctly
TEST(ParseHashtag, SimpleParse) {
  std::string text = "Hello this is a #hashtag #caw #faz";
  std::vector<std::string> hashtags = CawFunction::GetHashtags(text);
  ASSERT_EQ(hashtags.size(), 3);
  ASSERT_EQ(hashtags[0], "hashtag");
  ASSERT_EQ(hashtags[1], "caw");
  ASSERT_EQ(hashtags[2], "faz");
}

// A little more challenging test for the parse function
TEST(ParseHashtag, NonSimpleParse) {
  std::string text = "Hello this is a #hashtag#caw''''''#faz Ok testing #cawfaz";
  std::vector<std::string> hashtags = CawFunction::GetHashtags(text);
  ASSERT_EQ(hashtags.size(), 4);
  ASSERT_EQ(hashtags[0], "hashtag");
  ASSERT_EQ(hashtags[1], "caw");
  ASSERT_EQ(hashtags[2], "faz");
  ASSERT_EQ(hashtags[3], "cawfaz");
}

// A complex test for the parse function
TEST(ParseHashtag, ComplexParse) {
  std::string text = "*hashtag#hashtag#caw'''faz..#";
  std::vector<std::string> hashtags = CawFunction::GetHashtags(text);
  ASSERT_EQ(hashtags.size(), 2);
  ASSERT_EQ(hashtags[0], "hashtag");
  ASSERT_EQ(hashtags[1], "caw");
}
}  // namespace
