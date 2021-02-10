cmake_minimum_required(VERSION 3.16)
project(csci499_williamsuppiger VERSION 1.0.0 LANGUAGES CXX)
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
# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


enable_testing()
add_executable(key_value_test src/key_value_test.cc src/key_value.cc)
target_link_libraries(key_value_test gtest_main)

# add_executable(key_value_test src/key_value_test.cc src/key_value.cc ${kv_proto_srcs})
# target_link_libraries(key_value_test gtest_main ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

