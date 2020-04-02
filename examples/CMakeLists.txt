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