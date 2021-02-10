// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value_server.h"

#include <glog/logging.h>

#include <cstdio>
#include <string>
#include <vector>

namespace csci499 {

Status KeyValueServer::put(ServerContext* context, const PutRequest* request,
                           PutReply* reply) {
  storage_.Put(request->key(), request->value());
  LOG(INFO) << "rpc put key value pair " << request->key() << " "
            << request->value() << "\n";
  return Status::OK;
}

Status KeyValueServer::get(ServerContext* context,
           ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest request;
  stream->Read(&request);  // retrieve only the first request from the stream
  std::string key = request.key();
  std::vector<std::string> values(storage_.Get(key));
  for (auto v : values) {
    GetReply reply;
    reply.set_value(v);
    stream->Write(reply);
  }
  LOG(INFO) << "rpc get request on key " << key << "\n";
  return Status::OK;
}

Status KeyValueServer::remove(ServerContext* context,
                              const RemoveRequest* request,
                              RemoveReply* reply) {
  storage_.Remove(request->key());
  LOG(INFO) << "rpc removed key " << request->key() << "\n";
  return Status::OK;
}

}  // namespace csci499
