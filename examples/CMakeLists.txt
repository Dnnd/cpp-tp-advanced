add_executable(process-echo process-echo/main.cpp)
target_link_libraries(process-echo process)

add_executable(echo echo/main.cpp)

add_executable(logger-example logger-example/main.cpp)
target_link_libraries(logger-example log)

add_executable(echo-client tcp-example/echo-client.cpp)
target_include_directories(echo-client PUBLIC tcp-example/include)
target_link_libraries(echo-client tcp)

add_executable(echo-server tcp-example/echo-server.cpp)
target_include_directories(echo-server PUBLIC tcp-example/include)
target_link_libraries(echo-server tcp)

add_executable(async-echo-server async-tcp-example/async-echo.cpp)
target_include_directories(async-echo-server PUBLIC async-tcp-example/include)
target_link_libraries(async-echo-server tcp)

add_executable(http-example async-tcp-example/async-http-example.cpp)
target_include_directories(http-example PUBLIC async-tcp-example/include)
target_link_libraries(http-example tcp)

add_executable(shmem-example shmem-example/main.cpp)
target_link_libraries(shmem-example shmem pthread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(http-server-example http-example/http.cpp)

target_include_directories(http-server-example PUBLIC http-example/include)
target_link_libraries(http-server-example http Threads::Threads)

add_executable(kvstore-http kvstore-http/kvstore.cpp)
target_include_directories(kvstore-http PUBLIC kvstore-http/include)
target_link_libraries(kvstore-http http kvstore Threads::Threads)

add_executable(kvstore-generator kvstore-http/generator.cpp)
target_include_directories(kvstore-generator PUBLIC kvstore-http/include)
target_link_libraries(kvstore-generator kvstore)