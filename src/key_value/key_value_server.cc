// Copyright (c) 2021, USC
// All rights reserved.

#include "key_value_server.h"

#include <glog/logging.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace csci499 {

KeyValueServer::KeyValueServer() : storage_(), storage_file_() {}

KeyValueServer::KeyValueServer(std::string filename)
    : storage_(), storage_file_(filename) {
  std::ifstream input(storage_file_);
  if (input.is_open()) {  // read file if exists and store in snapshot
    KeyValueSnapshot snapshot;
    snapshot.ParseFromIstream(&input);
    storage_.LoadSnapshot(snapshot);
  }
  input.close();
}

Status KeyValueServer::put(ServerContext* context, const PutRequest* request,
                           PutReply* reply) {
  storage_.Put(request->key(), request->value());
  LOG(INFO) << "rpc put key value pair " << request->key() << " "
            << request->value();
  StoreSnapshot(storage_file_);
  return Status::OK;
}

Status KeyValueServer::get(ServerContext* context,
                           ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest request;
  stream->Read(&request);  // retrieve only the first request from the stream
  const std::string key = request.key();
  std::vector<std::string> values(storage_.Get(key));
  for (auto v : values) {
    GetReply reply;
    reply.set_value(v);
    stream->Write(reply);
  }
  LOG(INFO) << "rpc get request on key " << key;
  return Status::OK;
}

Status KeyValueServer::remove(ServerContext* context,
                              const RemoveRequest* request,
                              RemoveReply* reply) {
  storage_.Remove(request->key());
  LOG(INFO) << "rpc removed key " << request->key();
  StoreSnapshot(storage_file_);
  return Status::OK;
}

bool KeyValueServer::StoreSnapshot(const std::string filename) {
  if (!filename.empty()) {
    KeyValueSnapshot snapshot;
    storage_.CreateSnapshot(snapshot);  // get snapshot of DB from kv storage
    std::ofstream output;
    output.open(filename);
    if (output.is_open() && snapshot.SerializeToOstream(&output)) {
      LOG(INFO) << "stored kv snapshot to file";
      output.close();
    }
    return true;
  } else {
    LOG(WARNING) << "There was an error storing or serializing the current kv";
    return false;
  }
}

}  // namespace csci499
