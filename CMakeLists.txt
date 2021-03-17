cmake_minimum_required(VERSION 3.16)
project(csci499_williamsuppiger VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

# add library gtest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

# add library gRPC
FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.28.0
  )
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)
message(STATUS "Using gRPC")
set(_GRPC_GRPCPP grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)

# add GLOG library
FetchContent_Declare(
  glog
  GIT_REPOSITORY https://github.com/google/glog
  GIT_TAG        v0.4.0
  )
FetchContent_MakeAvailable(glog)

# aadd gflags library
FetchContent_Declare(
  gflags
  GIT_REPOSITORY https://github.com/gflags/gflags
  GIT_TAG        v2.2.2
  )
FetchContent_MakeAvailable(gflags)

# protobuf setup
set(protobuf_MODULE_COMPATIBLE TRUE)
message(STATUS "Using protobuf")
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)

# key value proto setup
get_filename_component(kv_proto "protos/key_value.proto" ABSOLUTE)
get_filename_component(kv_proto_path "${kv_proto}" PATH)
# generated files
set(kv_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/key_value.pb.cc")
set(kv_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/key_value.pb.h")
set(kv_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/key_value.grpc.pb.cc")
set(kv_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/key_value.grpc.pb.h")
add_custom_command(
      OUTPUT "${kv_proto_srcs}" "${kv_proto_hdrs}" "${kv_grpc_srcs}" "${kv_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${kv_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${kv_proto}"
      DEPENDS "${kv_proto}")

# faz proto setup
get_filename_component(faz_proto "protos/faz.proto" ABSOLUTE)
get_filename_component(faz_proto_path "${faz_proto}" PATH)
# generated files
set(faz_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.cc")
set(faz_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.h")
set(faz_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.cc")
set(faz_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.h")
add_custom_command(
      OUTPUT "${faz_proto_srcs}" "${faz_proto_hdrs}" "${faz_grpc_srcs}" "${faz_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${faz_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${faz_proto}"
      DEPENDS "${faz_proto}")

# caw proto setup
get_filename_component(caw_proto "protos/caw.proto" ABSOLUTE)
get_filename_component(caw_proto_path "${caw_proto}" PATH)
# generated files
set(caw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.cc")
set(caw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.h")
set(caw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/caw.grpc.pb.cc")
set(caw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/caw.grpc.pb.h")
add_custom_command(
      OUTPUT "${caw_proto_srcs}" "${caw_proto_hdrs}" "${caw_grpc_srcs}" "${caw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${caw_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${caw_proto}"
      DEPENDS "${caw_proto}")

# kv persist proto setup
get_filename_component(kv_persist_proto "protos/kv_persist.proto" ABSOLUTE)
get_filename_component(kv_persist_proto_path "${kv_persist_proto}" PATH)
# generated files
set(kv_persist_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/kv_persist.pb.cc")
set(kv_persist_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/kv_persist.pb.h")
set(kv_persist_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/kv_persist.grpc.pb.cc")
set(kv_persist_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/kv_persist.grpc.pb.h")
add_custom_command(
      OUTPUT "${kv_persist_proto_srcs}" "${kv_persist_proto_hdrs}" "${kv_persist_grpc_srcs}" "${kv_persist_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${kv_persist_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${kv_persist_proto}"
      DEPENDS "${kv_persist_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# setup directories
set (KV_DIR "src/key_value")
set (FAZ_DIR "src/faz")
set (CAW_DIR "src/caw")

add_executable(key_value_test ${KV_DIR}/key_value_test.cc ${KV_DIR}/key_value.cc ${kv_persist_proto_srcs})
target_link_libraries(key_value_test ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gtest_main glog::glog)

add_executable(key_value_server ${KV_DIR}/key_value_run.cc ${KV_DIR}/key_value_server.cc ${KV_DIR}/key_value.cc ${kv_proto_srcs} ${kv_grpc_srcs} ${kv_persist_proto_srcs})
target_link_libraries(key_value_server ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} glog::glog gflags )

add_executable(faz_server ${FAZ_DIR}/faz_run.cc ${FAZ_DIR}/faz_server.cc ${FAZ_DIR}/faz_client.cc ${CAW_DIR}/caw_function.cc ${KV_DIR}/key_value_client.cc ${kv_proto_srcs} ${kv_grpc_srcs} ${faz_proto_srcs} ${faz_grpc_srcs} ${caw_proto_srcs} ${caw_grpc_srcs})
target_link_libraries(faz_server ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} glog::glog gflags)

add_executable(caw_function_test ${CAW_DIR}/caw_function_test.cc ${CAW_DIR}/caw_function.cc ${KV_DIR}/key_value_server.cc ${KV_DIR}/key_value_client.cc ${KV_DIR}/key_value.cc ${kv_proto_srcs} ${kv_grpc_srcs} ${caw_proto_srcs} ${caw_grpc_srcs} ${kv_persist_proto_srcs})
target_link_libraries(caw_function_test gtest_main glog::glog ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(caw ${CAW_DIR}/caw.cc ${FAZ_DIR}/faz_client.cc ${faz_proto_srcs} ${faz_grpc_srcs} ${caw_proto_srcs} ${caw_grpc_srcs})
target_link_libraries(caw gflags glog::glog ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
