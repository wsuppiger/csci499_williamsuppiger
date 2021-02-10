// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value_server.h"

#include <glog/logging.h>

#include <cstdio>
#include <string>
#include <vector>

namespace backend {

Status KeyValueServer::put(ServerContext* context, const PutRequest* request,
                           PutReply* reply) override {
  storage_.Put(PutRequest->key(), PutRequest->value());
  LOG(INFO) << "rpc put key value pair " << PutRequest->key() << " "
            << PutRequest->value() << "\n";
  return Status::OK;
}

Status get(ServerContext* context,
           ServerReaderWriter<GetReply, GetRequest>* stream) override {
  GetRequest request;
  stream->Read(&request);  // retrieve only the first request from the stream
  std::string key = request->key();
  std::vector<std::string> values(storage_.Get(key));
  for (auto v : values) {
    stream->Write(v);
  }
  LOG(INFO) << "rpc get request on key " << key << "\n";
  return Status::OK;
}

Status KeyValueServer::remove(ServerContext* context,
                              const RemoveRequest* request,
                              RemoveReply* reply) override {
  storage_.Remove(request->key());
  LOG(INFO) << "rpc removed key " << request->key() << "\n";
  return Status::OK;
}

}  // namespace backend
