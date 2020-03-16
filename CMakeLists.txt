cmake_minimum_required(VERSION 3.15)
project(process_wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

add_subdirectory(examples)

add_library(process STATIC lib/process/process.cpp lib/process/descriptor.cpp lib/process/pipe.cpp)
target_include_directories(process PUBLIC include)

add_executable(echo src/echo/main.cpp src/echo/main.cpp)