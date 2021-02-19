// Copyright (c) 2021, USC
// All rights reserved.

#include "caw_function.h"

#include <glog/logging.h>

#include <vector>

namespace csci499 {

// static map to functions has to be initialized outside of the class
std::unordered_map<std::string,
                   std::function<CawFuncReply(const Any&, KeyValueInterface&)> >
    CawFunction::function_map_ = {{"registeruser", &CawFunction::RegisterUser},
                                  {"caw", &CawFunction::Caw},
                                  {"follow", &CawFunction::Follow},
                                  {"read", &CawFunction::Read},
                                  {"profile", &CawFunction::Profile}};

CawFuncReply CawFunction::RegisterUser(const Any& payload,
                                       KeyValueInterface& kv) {
  RegisteruserRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();
  if (username.empty()) {
    LOG(WARNING) << "username cannot be blank";
    return {Status(StatusCode::INVALID_ARGUMENT, "username cannot be blank")};
  } else if (UserExists(username, kv)) {
    LOG(WARNING) << "tried to duplicate user " << username;
    return {Status(StatusCode::ALREADY_EXISTS, "this user alread exists")};
  }

  kv.Put("users", username);
  LOG(INFO) << "created user " << username;
  return {Status(StatusCode::OK, "successfully registered user " + username)};
}

CawFuncReply CawFunction::Caw(const Any& payload, KeyValueInterface& kv) {
  // wait for prof email
  return {Status::OK};
}

CawFuncReply CawFunction::Follow(const Any& payload, KeyValueInterface& kv) {
  FollowRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();
  std::string to_follow = request.to_follow();
  std::vector<std::string> ufollowing = kv.Get("ufollowing-" + username);
  if (!UserExists(username, kv) || !UserExists(to_follow, kv)) {
    LOG(WARNING) << "one of the users does not exist";
    return {Status(StatusCode::UNAVAILABLE,
                   "at least one of the users does not exist")};
  } else if (username == to_follow) {
    LOG(WARNING) << "user " + username + " tried to follow themself";
    return {Status(StatusCode::UNAVAILABLE, "a user cannot follow themself")};
  } else if (std::find(ufollowing.begin(), ufollowing.end(), to_follow) !=
             ufollowing.end()) {
    LOG(WARNING) << "user " + username + " is already following " << to_follow;
    return {Status(StatusCode::UNAVAILABLE,
                   username + " already follows " + to_follow)};
  }

  // key ufollowing-<username> stores all the users <username> is following
  // key ufollowers-<username> stores all the followers for a given <username>
  kv.Put("ufollowing-" + username, to_follow);
  kv.Put("ufollowers-" + to_follow, username);
  LOG(INFO) << "user " << username << "followed " << to_follow;
  return {
      Status(StatusCode::OK, "user " + username + " followed " + to_follow)};
}

CawFuncReply CawFunction::Read(const Any& payload, KeyValueInterface& kv) {
  // wait for prof email
  return {Status::OK};
}

CawFuncReply CawFunction::Profile(const Any& payload, KeyValueInterface& kv) {
  ProfileRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();
  if (!UserExists(username, kv)) {
    LOG(WARNING) << "the user does not exist";
    return {Status(StatusCode::UNAVAILABLE, "the user does not exist")};
  }
  std::vector<std::string> followers = kv.Get("ufollowers-" + username);
  std::vector<std::string> following = kv.Get("ufollowing-" + username);
  ProfileReply reply;
  for (auto f : followers) {
    reply.add_followers(f);
  }
  for (auto f : following) {
    reply.add_following(f);
  }
  return {Status::OK, reply.SerializeAsString()};
}

bool CawFunction::UserExists(const std::string& username,
                             KeyValueInterface& kv) {
  std::vector<std::string> users = kv.Get("users");
  return std::find(users.begin(), users.end(), username) != users.end();
}

}  // namespace csci499
