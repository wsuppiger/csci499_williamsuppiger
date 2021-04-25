// Copyright (c) 2021, USC
// All rights reserved.

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <stdio.h>
#include <time.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stack>

#include "../faz/faz_client.h"
#include "caw.grpc.pb.h"
#include "event_type.h"
#include "faz.grpc.pb.h"

DEFINE_bool(hookall, false,
            "Hooks all Caw functions in the caw function class");
DEFINE_bool(unhookall, false,
            "Unhooks all Caw functions in the caw function class");
DEFINE_string(registeruser, "", "register user for a Caw account");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(caw, "", "Creates a new caw with the given text");
DEFINE_string(reply, "",
              "Indicates that the new caw is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the caw thread starting at the given id");
DEFINE_bool(profile, false,
            "Gets the user’s profile of following and followers");
DEFINE_string(stream, "",
            "Streams all new caws containing the string passed in");

// wrapper namespace to avoid clashing with backend enum event types
namespace input {
// each type of command line input possible
enum CommandInput {
  kInvalidCommand = -1,
  kHookAll,
  kUnhookAll,
  kRegisteruser,
  kCaw,
  kReply,
  kFollow,
  kRead,
  kProfile,
  kStream,
};
}  // namespace input

// validate flag and return the command input that was provided by the flags
int GetCommand();

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  csci499::FazClient client(
      grpc::CreateChannel("0.0.0.0:50000", grpc::InsecureChannelCredentials()));
  int command = GetCommand();
  const std::map<int, std::string> event_map = {{kRegisteruser, "registeruser"},
                                                {kCaw, "caw"},
                                                {kFollow, "follow"},
                                                {kRead, "read"},
                                                {kProfile, "profile"},
                                                {kStream, "stream"}};
  auto payload = google::protobuf::Any();

  if (command == input::kInvalidCommand) {
    std::cout << "Invalid command.  Use --help for info about the commands."
              << std::endl;
  } else if (command == input::kHookAll) {
    for (auto event : event_map) {
      bool ok = client.Hook(event.first, event.second);
      if (!ok) {
        std::cout << "There was an errror hooking event " << event.second
                  << std::endl;
        return 1;
      }
    }
    std::cout << "All events successfully hooked." << std::endl;
  } else if (command == input::kUnhookAll) {
    for (auto event : event_map) {
      bool ok = client.Unhook(event.first);
      if (!ok) {
        std::cout << "There was an errror unhooking event " << event.second
                  << std::endl;
        return 1;
      }
    }
    std::cout << "All events successfully unhooked." << std::endl;
  } else if (command == input::kRegisteruser) {
    caw::RegisteruserRequest request;
    request.set_username(FLAGS_registeruser);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Registering user" << FLAGS_registeruser;
    // NOTE: kRegisteruser is from event_type.h and not the input namespace
    grpc::Status status = client.Event(kRegisteruser, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing registeruser: "
                   << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      LOG(INFO) << "registered user " << FLAGS_registeruser;
      std::cout << FLAGS_registeruser << " is successfully registered."
                << std::endl;
    }
  } else if (command == input::kCaw) {
    caw::CawRequest request;
    request.set_username(FLAGS_user);
    request.set_text(FLAGS_caw);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Creating Caw for user " << FLAGS_user << " with message "
              << FLAGS_caw;
    grpc::Status status = client.Event(kCaw, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing caw: " << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      caw::CawReply reply;
      google::protobuf::Any(event_reply.payload()).UnpackTo(&reply);
      caw::Caw caw = reply.caw();
      LOG(INFO) << "created caw with ID: " << caw.id();
      std::cout << "Created caw with ID: " << caw.id() << std::endl;
    }
  } else if (command == input::kReply) {
    caw::CawRequest request;
    request.set_username(FLAGS_user);
    request.set_text(FLAGS_caw);
    request.set_parent_id(FLAGS_reply);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Replying to Caw " << FLAGS_reply << " for user " << FLAGS_user
              << " with message " << FLAGS_caw;
    grpc::Status status = client.Event(kCaw, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing caw: " << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      caw::CawReply reply;
      google::protobuf::Any(event_reply.payload()).UnpackTo(&reply);
      caw::Caw caw = reply.caw();
      LOG(INFO) << "created caw with ID: " << caw.id() << " in reply to caw id"
                << caw.parent_id();
      std::cout << "Created caw with ID: " << caw.id() << " in reply to caw id "
                << caw.parent_id() << std::endl;
    }
  } else if (command == input::kFollow) {
    caw::FollowRequest request;
    request.set_username(FLAGS_user);
    request.set_to_follow(FLAGS_follow);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Follow request from " << FLAGS_user << " to " << FLAGS_follow;
    grpc::Status status = client.Event(kFollow, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing follow: "
                   << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      LOG(INFO) << "Follow request successful from " << FLAGS_user << " to "
                << FLAGS_follow;
      std::cout << FLAGS_user << " is now following " << FLAGS_follow
                << std::endl;
    }
  } else if (command == input::kRead) {
    caw::ReadRequest request;
    request.set_caw_id(FLAGS_read);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Read request for caw " << FLAGS_read;
    grpc::Status status = client.Event(kRead, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing read: " << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      caw::ReadReply reply;
      google::protobuf::Any(event_reply.payload()).UnpackTo(&reply);
      auto caws = reply.caws();
      LOG(INFO) << "Successfully read Caw " << FLAGS_read;
      // Caws are returned in DFS pattern, so use stack to format indentation
      std::stack<caw::Caw> caw_stack;
      for (auto caw : caws) {
        while (!caw_stack.empty() && caw.parent_id() != caw_stack.top().id()) {
          caw_stack.pop();
        }
        // print out caw in indented pretty format
        std::chrono::seconds time_seconds(caw.timestamp().seconds());
        std::chrono::time_point<std::chrono::system_clock> tp(time_seconds);
        auto t_c = std::chrono::system_clock::to_time_t(tp);

        std::cout << "[" << std::put_time(std::localtime(&t_c), "%F %T") << "]"
                  << std::string(caw_stack.size() * 4, '-') << " #" << caw.id()
                  << " " << caw.username() << ": " << caw.text() << std::endl;
        caw_stack.push(caw);
      }
    }
  } else if (command == input::kProfile) {
    caw::ProfileRequest request;
    request.set_username(FLAGS_user);
    payload.PackFrom(request);
    faz::EventReply event_reply;
    LOG(INFO) << "Profile request from " << FLAGS_user;
    grpc::Status status = client.Event(kProfile, payload, event_reply);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing profile: "
                   << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    } else {
      LOG(INFO) << "Profile request successful from " << FLAGS_user;
      caw::ProfileReply reply;
      google::protobuf::Any(event_reply.payload()).UnpackTo(&reply);
      auto followers = reply.followers();
      auto following = reply.following();
      std::cout << "followers: ";
      for (auto follower : followers) {
        std::cout << follower;
        if (follower != followers[followers.size() - 1]) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl << "following: ";
      for (auto follow : following) {
        std::cout << follow;
        if (follow != following[following.size() - 1]) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
    }
  } else if (command == input::kStream) {
    caw::StreamRequest request;
    request.set_hashtag(FLAGS_stream);
    payload.PackFrom(request);
    std::function<void(faz::EventReply)> print_caw = [](faz::EventReply response) {
      caw::CawReply reply;
      response.payload().UnpackTo(&reply);
      caw::Caw caw = reply.caw();
      std::cout << "[" << std::put_time(std::localtime(&t_c), "%F %T") << "]"
                  << std::string(caw_stack.size() * 4, '-') << " #" << caw.id()
                  << " " << caw.username() << ": " << caw.text() << std::endl; 
    };
    grpc::Status status = client.Stream(kStream, payload, print_caw);
    if (!status.ok()) {
      LOG(WARNING) << "error while executing stream: " << status.error_message();
      std::cout << status.error_message() << std::endl;
      return 1;
    }
  }
  return 0;
}

int GetCommand() {
  // boolean values for if each field contains a value
  bool registeruser = !FLAGS_registeruser.empty(), user = !FLAGS_user.empty(),
       caw = !FLAGS_caw.empty(), reply = !FLAGS_reply.empty(),
       follow = !FLAGS_follow.empty(), read = !FLAGS_read.empty(), 
       stream = !FLAGS_stream.empty(), profile = FLAGS_profile,
       hook = FLAGS_hookall, unhook = FLAGS_unhookall;
  if (!registeruser && !user && !caw && !reply && !follow && !read &&
      !profile && !stream) {
    if (hook && !unhook) {
      return input::kHookAll;  // ./caw --hookall
    } else if (!hook && unhook) {
      return input::kUnhookAll;  // ./caw --unhookall
    }
  } else if (!hook && !unhook) {
    if (registeruser && !user && !caw && !reply && !follow && !read &&
        !profile && !stream) {
      return input::kRegisteruser;  // ./caw --registeruser <username>
    } else if (!registeruser && user) {
      if (caw && !follow && !read && !profile && !stream) {
        if (caw && !reply) {
          return input::kCaw;  // ./caw --user <username> --caw <caw text>
        } else if (caw && reply) {
          return input::kReply;  // ./caw --user <username> --caw <caw text>
                                 // --reply <reply caw id>
        }
      } else if (!caw && follow && !read && !profile && !stream) {
        return input::kFollow;  // ./caw --user <username> --follow <username>
      } else if (!caw && !follow && read && !profile && !stream) {
        return input::kRead;  // ./caw --user <username> --read <caw id>
      } else if (!caw && !follow && !read && profile && !stream) {
        return input::kProfile;  // ./caw --user <username> --profile
      } else if (!caw && !follow && !read && !profile && stream) {
        return input::kStream; // ./caw --user <username> --stream <hashtag>
      }
    }
  }
  return input::kInvalidCommand;
}
