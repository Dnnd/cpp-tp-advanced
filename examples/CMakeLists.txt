add_executable(process-echo process-echo/main.cpp)
target_link_libraries(process-echo process)

add_executable(echo echo/main.cpp)

add_executable(logger-example logger-example/main.cpp)
target_link_libraries(logger-example log)