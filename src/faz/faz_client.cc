// Copyright (c) 2021, USC
// All rights reserved.

#include "faz_client.h"

#include "glog/logging.h"

namespace csci499 {

bool FazClient::Hook(int event_type, const std::string& event_function) {
  HookRequest request;
  request.set_event_type(event_type);
  request.set_event_function(event_function);

  ClientContext context;
  HookReply reply;
  Status status = stub_->hook(&context, request, &reply);

  if (status.ok()) {
    LOG(INFO) << "client call to hook function" << event_function
              << "with type:" << event_type;
    return true;
  } else {
    LOG(WARNING) << "error in client hook call: " << status.error_code();
    return false;
  }
}  // namespace csci499

bool FazClient::Unhook(int event_type) {
  UnhookRequest request;
  request.set_event_type(event_type);

  ClientContext context;
  UnhookReply reply;
  Status status = stub_->unhook(&context, request, &reply);

  if (status.ok()) {
    LOG(INFO) << "client call to unhook event type:" << event_type;
    return true;
  } else {
    LOG(WARNING) << "error in client unhook call: " << status.error_code();
    return false;
  }
}

Status FazClient::Event(int event_type, Any& payload, EventReply& reply) {
  EventRequest request;
  request.set_event_type(event_type);
  *request.mutable_payload() = payload;

  ClientContext context;
  Status status = stub_->event(&context, request, &reply);
  return status;
}

Status FazClient::Stream(int event_type, Any& payload, EventReply& reply) {
  // Pack request and call server
  return Status::OK;
}

}  // namespace csci499
