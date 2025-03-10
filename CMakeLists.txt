
cmake_minimum_required(VERSION 3.16.3)

set(PROGRAM nano)
set(LIBNAME libnano)
set(TEST_EXE nano_tests)

project(nano
  VERSION 0.0.0
  DESCRIPTION "The nano chess engine"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###################################################################
# Options
###################################################################

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# Prevent GoogleTest from overriding compiler options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

###################################################################
# Build Nano
###################################################################

###################################################################
# Source Files
###################################################################
set(SRC_FILES
  src/bitboards.cpp
  src/magics.cpp
  src/zobrist.cpp
)

###################################################################
# Build Configuration
###################################################################
if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "Build configuration not set, setting configuration to Release.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build configuration type: Debug Release.")
endif()

###################################################################
# Compilation Options
###################################################################
if(WIN32)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /W3 /GR /EHsc /D_64BIT /D_CONSOLE /D_UNICODE /D_WIN32 /D_WIN64 /D_MSC_VER=1939 /std:c++20 /GS /GL /W3 /Gy /Zi /Gm- /O2 /Ob2 /Zc:inline /fp:precise /GT /WX- /Ot /FC /Oi /MD")
  elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /JMC /EHsc /GS /W3 /ZI /Gm- /Od /Zc:inline /fp:precise /WX- /RTC1 /Gd /MDd /FC /D_DEBUG /D_64BIT /D_CONSOLE /D_UNICODE /D_WIN32 /D_WIN64 /D_MSC_VER=1939 /std:c++20")
  endif()
else()
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fomit-frame-pointer -fstrict-aliasing -ffast-math -O3 -std=c++20 -D_64BIT -D_CONSOLE -D_UNICODE -mavx")
  elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa -Wall -g -ggdb -O0 -std=c++20 -D_DEBUG -D_64BIT -D_CONSOLE -D_UNICODE")
  endif()
endif()

# ========================================
# 1. Create a static library for gtest
# ========================================
add_library(${LIBNAME} STATIC ${SRC_FILES})
target_include_directories(${LIBNAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# ================================
# 2. Build the Chess Engine Executable
# ================================
add_executable(${PROGRAM} src/nano.cpp)

# Link chessengine static library to the main executable
target_link_libraries(${PROGRAM} PRIVATE ${LIBNAME})

###################################################################
# Build Tests
###################################################################

set(TST_FILES
  tests/test_magics.cpp
)

# Add the test executable
add_executable(${TEST_EXE} ${TST_FILES})

# Link against Google Test
target_link_libraries(${TEST_EXE} PRIVATE gtest_main ${LIBNAME})