cmake_minimum_required(VERSION 3.15)
project(process_wrapper)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(examples)
set(PROCESS_SOURCES lib/process/process.cpp lib/process/descriptor.cpp lib/process/pipe.cpp)

add_library(process STATIC ${PROCESS_SOURCES})
target_include_directories(process PUBLIC include)

set(LOG_SOURCES lib/log/base_logger.cpp
        lib/log/level.cpp
        lib/log/stderr_logger.cpp
        lib/log/stdout_logger.cpp
        lib/log/file_logger.cpp
        lib/log/logger.cpp
        lib/log/message.cpp
        lib/log/exceptions.cpp)

add_library(log STATIC ${LOG_SOURCES})
target_include_directories(log PUBLIC include)