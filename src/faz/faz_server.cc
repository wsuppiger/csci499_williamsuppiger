// Copyright (c) 2021, USC
// All rights reserved.

#include "faz_server.h"

#include <glog/logging.h>

#include <string>
#include <vector>

namespace csci499 {

Status FazServer::hook(ServerContext* context, const HookRequest* request,
                       HookReply* reply) {
  const std::string event_type = std::to_string(request->event_type());
  const std::string event_function = request->event_function();
  kv_.Put(event_type, event_function);
  LOG(INFO) << "hooked type " << event_type << "with function "
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
    event_function = get_event[0];
  }

  auto caw_method = caw_.GetFunction(event_function);
  if (caw_method == nullptr) {
    LOG(WARNING) << "not found in CawFunction class for function "
                 << event_function;
    return Status(StatusCode::NOT_FOUND, "function call not found");
  }
  CawFuncReply func_reply = (caw_.*(caw_method))(request->payload());

  Any* any = new Any();
  switch (event_type) {
    case 1:  // registeruser
      break;
    case 2:  // caw
      CawReply caw_reply;
      caw_reply.ParseFromString(func_reply.message);
      any->PackFrom(caw_reply);
      break;
  }
  reply->set_allocated_payload(any);
  return func_reply.status;
}
}  // namespace csci499
