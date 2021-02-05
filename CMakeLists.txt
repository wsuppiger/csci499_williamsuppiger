cmake_minimum_required(VERSION 3.16)
project(csci499_williamsuppiger VERSION 1.0.0 LANGUAGES CXX)
include(FetchContent)

enable_testing()
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

# add_executable(app
#   src/key_value.cc)

add_executable(key_value_test src/key_value.cc)
target_link_libraries(key_value_test gtest_main)

# target_compile_features(app PRIVATE cxx_std_17)
# target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)


