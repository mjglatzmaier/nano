
cmake_minimum_required(VERSION 3.16.3)

set(PROGRAM nano)

project(nano
  VERSION 0.0.0
  DESCRIPTION "The nano chess engine"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Prevent GoogleTest from overriding compiler options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

############################################
# Build Tests
############################################

# Add the test executable
#add_executable(nano_tests movegen_tests.cpp eval_tests.cpp)

# Link against Google Test
#target_link_libraries(run_tests PRIVATE gtest gtest_main)

# Enable test discovery
#enable_testing()
#add_test(NAME RunAllTests COMMAND run_tests)
