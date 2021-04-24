// Copyright (c) 2021, USC
// All rights reserved.

#include "faz_server.h"

#include <glog/logging.h>

#include <string>
#include <vector>

#include "../caw/event_type.h"

namespace csci499 {
Status FazServer::hook(ServerContext* context, const HookRequest* request,
                       HookReply* reply) {
  const std::string event_type = std::to_string(request->event_type());
  const std::string event_function = request->event_function();
  kv_.Put(event_type, event_function);
  LOG(INFO) << "hooked type " << event_type << " with function "
            << event_function;
  return Status::OK;
}

Status FazServer::unhook(ServerContext* context, const UnhookRequest* request,
                         UnhookReply* reply) {
  const std::string event_type = std::to_string(request->event_type());
  kv_.Remove(event_type);
  LOG(INFO) << "unhooked type " << event_type;
  return Status::OK;
}

Status FazServer::event(ServerContext* context, const EventRequest* request,
                        EventReply* reply) {
  const int event_type = request->event_type();
  std::vector<std::string> get_event = kv_.Get(std::to_string(event_type));
  std::string event_function;
  if (get_event.empty()) {
    LOG(WARNING) << "event is unhooked for event type " << event_type;
    return Status(StatusCode::NOT_FOUND, "event in not hooked");
  } else {
    event_function = get_event.back();
  }

  auto caw_method = CawFunction::function_map_[event_function];
  if (caw_method == nullptr) {
    LOG(WARNING) << "not found in CawFunction class for function "
                 << event_function;
    return Status(StatusCode::NOT_FOUND, "function call not found");
  }
  CawFuncReply func_reply = caw_method(request->payload(), kv_);

  Any any = Any();
  if (event_type == EventType::kRegisteruser) {
    RegisteruserReply reply;
    reply.ParseFromString(func_reply.message);
    any.PackFrom(reply);
  } else if (event_type == EventType::kCaw) {
    CawReply reply;
    reply.ParseFromString(func_reply.message);
    any.PackFrom(reply);
  } else if (event_type == EventType::kFollow) {
    FollowReply reply;
    reply.ParseFromString(func_reply.message);
    any.PackFrom(reply);
  } else if (event_type == EventType::kRead) {
    ReadReply reply;
    reply.ParseFromString(func_reply.message);
    any.PackFrom(reply);
  } else if (event_type == EventType::kProfile) {
    ProfileReply reply;
    reply.ParseFromString(func_reply.message);
    any.PackFrom(reply);
  }

  *reply->mutable_payload() = any;
  return func_reply.status;
}

Status FazServer::stream(ServerContext* context, const EventRequest* request, 
                ServerWriter<EventReply>* writer) {

  // This function calls the corresponding caw.h function
  return Status::OK;
}
}  // namespace csci499
