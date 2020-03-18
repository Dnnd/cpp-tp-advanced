cmake_minimum_required(VERSION 3.15)
project(process_wrapper)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(examples)
set(SOURCES lib/process/process.cpp lib/process/descriptor.cpp lib/process/pipe.cpp)
add_library(process STATIC ${SOURCES})
target_include_directories(process PUBLIC include)