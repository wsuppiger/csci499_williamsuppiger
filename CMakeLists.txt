cmake_minimum_required(VERSION 3.16)

project(csci499_williamsuppiger VERSION 1.0.0 LANGUAGES CXX)

add_executable(app
  src/keyValue.cc)

target_compile_features(app PRIVATE cxx_std_17)

target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)

