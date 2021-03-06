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
        lib/log/thread_safe_logger.cpp
        lib/log/logger.cpp
        lib/log/message.cpp
        lib/log/exceptions.cpp)

add_library(log STATIC ${LOG_SOURCES})
target_include_directories(log PUBLIC include)

set(TCP_SOURCES lib/tcp/connection.cpp
        lib/tcp/utils.cpp
        lib/tcp/server.cpp
        lib/tcp/exceptions.cpp
        lib/tcp/sockinfo.cpp
        lib/tcp/async_server.cpp
        lib/tcp/epoller.cpp
        lib/tcp/async_connection.cpp
        lib/tcp/events.cpp
        )
add_library(tcp STATIC ${TCP_SOURCES})
target_include_directories(tcp PUBLIC include)

set(SHMEM_SOURCE
        lib/shmem/shared_region.cpp
        lib/shmem/shmutex.cpp
        lib/shmem/exceptions.cpp
        include/shmem/shmallocator.hpp
        include/shmem/shmap.hpp
        lib/shmem/lockable_sem.cpp)
add_library(shmem STATIC ${SHMEM_SOURCE})
target_include_directories(shmem PUBLIC include)

set(HTTP_SOURCES
        lib/http/http_server.cpp
        lib/http/protocol.cpp
        lib/http/parser.cpp
        lib/http/utils.cpp
        lib/http/http_worker.cpp
        lib/http/exceptions.cpp
        lib/http/connection_handler.cpp
        lib/http/http_request.cpp
        lib/http/http_response.cpp

        )
add_library(http STATIC ${HTTP_SOURCES})
target_include_directories(http PUBLIC include)
target_link_libraries(http PUBLIC tcp coroutine)
target_link_libraries(http PUBLIC tcp coroutine log)

set(CORO_SOURCES
        lib/coroutine/coroutine.cpp)
add_library(coroutine STATIC ${CORO_SOURCES})
target_include_directories(coroutine PUBLIC include/coroutine)

set(KVSTORE_SOURCES
        lib/kvstore/file.cpp
        lib/kvstore/descriptor.cpp
        lib/kvstore/exceptions.cpp
        include/kvstore/index.hpp
        include/kvstore/store.hpp
        )
add_library(kvstore STATIC ${KVSTORE_SOURCES})
target_include_directories(kvstore PUBLIC include)