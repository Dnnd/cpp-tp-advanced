add_executable(process-echo process-echo/main.cpp)
target_link_libraries(process-echo PRIVATE process)
target_include_directories(process-echo PUBLIC include)