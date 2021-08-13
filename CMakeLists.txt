cmake_minimum_required(VERSION 3.16)
project(first)

set(CMAKE_CXX_STANDARD 14)

add_executable(lar-ws-chan-client main.cpp)
target_link_libraries(lar-ws-chan-client boost_system)
target_link_libraries(lar-ws-chan-client pthread)
target_link_libraries(lar-ws-chan-client boost_thread)
target_link_libraries(lar-ws-chan-client boost_chrono)