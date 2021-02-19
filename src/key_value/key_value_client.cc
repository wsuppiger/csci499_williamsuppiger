// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value_client.h"

#include <glog/logging.h>

namespace csci499 {
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

void KeyValueClient::Put(const std::string& key, const std::string& value) {
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  PutReply reply;
  ClientContext context;
  Status status = stub_->put(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Put request successful from client for key " << key;
  } else {
    LOG(WARNING) << "Put request failed from client for key " << key;
  }
}

std::vector<std::string> KeyValueClient::Get(const std::string& key) {
  GetRequest request;
  request.set_key(key);
  ClientContext context;
  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
      stub_->get(&context));
  stream->Write(request);
  stream->WritesDone();

  GetReply reply;
  std::vector<std::string> values;
  while (stream->Read(&reply)) {
    values.push_back(reply.value());
    LOG(INFO) << "Get request value returned: " << reply.value();
  }
  Status status = stream->Finish();
  if (status.ok()) {
    LOG(INFO) << "Get request successful from client for key " << key;
    return values;
  } else {
    LOG(WARNING) << "Get request failed from client for key " << key;
    return {};
  }
}

void KeyValueClient::Remove(const std::string& key) {
  RemoveRequest request;
  request.set_key(key);

  RemoveReply reply;
  ClientContext context;
  Status status = stub_->remove(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Remove request successful from client for key " << key;
  } else {
    LOG(WARNING) << "Remove request failed from client for key " << key;
  }
}

}  // namespace csci499
