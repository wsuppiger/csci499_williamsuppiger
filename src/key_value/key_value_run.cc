// Copyright (c) 2021, USC
// All rights reserved.

#include <glog/logging.h>

#include <string>

#include "key_value_server.h"

namespace csci499 {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

void RunKVServer() {
  std::string server_address("0.0.0.0:50001");
  KeyValueServer service;

  // listen on port 50001 with insecure credititals
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  // Assembling the server
  std::unique_ptr<Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server listening on port: " << server_address;

  server->Wait();
}

}  // namespace csci499

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  csci499::RunKVServer();
  return 0;
}
