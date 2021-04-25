// Copyright (c) 2021, USC
// All rights reserved.

#include "caw_function.h"

#include <glog/logging.h>
#include <sys/time.h>
#include <unistd.h>

#include <vector>

namespace csci499 {

// static map to functions has to be initialized outside of the class
std::unordered_map<std::string,
                   std::function<CawFuncReply(const Any&, KeyValueInterface&)> >
    CawFunction::function_map_ = {{"registeruser", &CawFunction::RegisterUser},
                                  {"caw", &CawFunction::CawCreate},
                                  {"follow", &CawFunction::Follow},
                                  {"read", &CawFunction::Read},
                                  {"profile", &CawFunction::Profile}};
                                  
std::unordered_map<std::string, std::function<CawFuncReply(const Any&,
                                              std::unordered_map<std::string, 
                                                std::vector<std::function<
                                                  bool(const Any&)>> >&)
                  > > CawFunction::stream_function_map_ = {{"stream", &CawFunction::Stream}};

// Declaring static lock
std::mutex CawFunction::lock_;

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

CawFuncReply CawFunction::CawCreate(const Any& payload, KeyValueInterface& kv) {
  CawRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();
  std::string text = request.text();
  std::string parent_id = request.parent_id();
  std::vector<std::string> prev_caws = kv.Get("caws");
  // the caw id for the current caw is the previous count of all caws before it
  std::string caw_id = std::to_string(prev_caws.size());
  if (!UserExists(username, kv)) {
    LOG(WARNING) << "the user does not exist";
    return {Status(StatusCode::UNAVAILABLE, "the user does not exist")};
  } else if (!parent_id.empty()) {
    if (std::find(prev_caws.begin(), prev_caws.end(), parent_id) ==
        prev_caws.end()) {
      LOG(WARNING) << "the parent caw does not exist";
      return {Status(StatusCode::UNAVAILABLE, "the parent caw does not exist")};
    } else {
      // store all children ids of a Caw in a key cawchildren-<parent-id>
      kv.Put("cawchildren-" + parent_id, caw_id);
    }
  }
  kv.Put("caws", caw_id);
  timeval curr_time;
  gettimeofday(&curr_time, NULL);
  Timestamp time;
  time.set_seconds(curr_time.tv_sec);
  time.set_useconds(curr_time.tv_usec);
  Caw caw;  // create caw message
  caw.set_username(username);
  caw.set_text(text);
  caw.set_id(caw_id);
  caw.set_parent_id(parent_id);
  caw.mutable_timestamp()->CopyFrom(time);
  // store serialized caw in key caw-<caw-id>
  kv.Put("caw-" + caw_id, caw.SerializeAsString());

  CawReply reply;
  reply.mutable_caw()->CopyFrom(caw);
  return {Status::OK, reply.SerializeAsString()};
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
  ReadRequest request;
  payload.UnpackTo(&request);
  std::string caw_id = request.caw_id();
  std::vector<std::string> caws = kv.Get("caws");
  if (std::find(caws.begin(), caws.end(), caw_id) == caws.end()) {
    LOG(WARNING) << "the caw does not exist";
    return {Status(StatusCode::UNAVAILABLE, "the caw does not exist")};
  }
  std::vector<Caw> caw_replys;
  // recursively find all children/replys to caw_id
  ReadReplys(caw_id, kv, caw_replys);
  ReadReply reply;
  for (int i = 0; i < caw_replys.size(); ++i) {
    Caw* c = reply.add_caws();
    c->CopyFrom(caw_replys[i]);
  }
  return {Status::OK, reply.SerializeAsString()};
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
  for (const auto& f : followers) {
    reply.add_followers(f);
  }
  for (const auto& f : following) {
    reply.add_following(f);
  }
  return {Status::OK, reply.SerializeAsString()};
}

CawFuncReply CawFunction::Stream(const Any& payload,
                                  std::unordered_map<std::string, 
                                    std::vector<std::function<bool(const Any&)>> >&
                                  current_streamers_) {
  CawReply reply; 
  payload.UnpackTo(&reply);
  Caw caw = reply.caw();
  std::string caw_text = caw.text();
  // Using a lock to prevent race conditions while 
  // multiple threads interract with the current_streamers_
  const std::lock_guard<std::mutex> lock(lock_);
  std::vector<std::string> hashtags = GetHashtags(caw_text);
  for (std::string hashtag : hashtags) {
    if (current_streamers_.find(hashtag) == current_streamers_.end()) {
      // If there are no streamers for this hashtag do nothing
      continue; 
    }
    // Else get the vector of streamers
    auto& hashtag_streamers = current_streamers_[hashtag];
    for (auto it = hashtag_streamers.begin(); it != hashtag_streamers.end();) {
      // We will pass in the any object that wraps
      // the CawReply to the callback function
      // this will be unwrapped and printed out to the streamers
      bool server_is_alive = (*it)(payload);
      if (!server_is_alive) {
        hashtag_streamers.erase(it);
      } else {
        ++it; 
      }
    }
  }
  return {Status::OK, reply.SerializeAsString()};
}

bool CawFunction::UserExists(const std::string& username,
                             KeyValueInterface& kv) {
  std::vector<std::string> users = kv.Get("users");
  return std::find(users.begin(), users.end(), username) != users.end();
}

void CawFunction::ReadReplys(const std::string& caw_id, KeyValueInterface& kv,
                             std::vector<Caw>& caws) {
  Caw caw;
  caw.ParseFromString(kv.Get("caw-" + caw_id)[0]);
  caws.push_back(caw);
  std::vector<std::string> children = kv.Get("cawchildren-" + caw_id);
  for (const auto& c : children) {
    ReadReplys(c, kv, caws);
  }
}

std::vector<std::string> CawFunction::GetHashtags(const std::string& message) {
  std::vector<std::string> hashtags; 
  string hashtag = ""; 
  int i=0; 
  while (i < message.size()) {
    if (message[i] == '#') {
      i++; 
      while (i < message.size() && std::isalnum(message[i])) {
        hashtag += message[i++];
      } // Stop at a space or some other non alphanum char
      if (hashtag != "") {
        hashtags.push_back(hashtag);
        hashtag = "";
      }
    } else {
      i++;
    }
  }
  return hashtags;
}

}  // namespace csci499
