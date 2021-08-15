cmake_minimum_required(VERSION 3.16)
project(first)

set(CMAKE_CXX_STANDARD 14)

add_executable(lar-ws-chan-client-sync main_sync.cpp)
target_link_libraries(lar-ws-chan-client-sync boost_system)
target_link_libraries(lar-ws-chan-client-sync pthread)
target_link_libraries(lar-ws-chan-client-sync boost_thread)
target_link_libraries(lar-ws-chan-client-sync boost_chrono)

add_executable(lar-ws-chan-client-async main_async.cpp)
target_link_libraries(lar-ws-chan-client-async boost_system)
target_link_libraries(lar-ws-chan-client-async pthread)
target_link_libraries(lar-ws-chan-client-async boost_thread)
target_link_libraries(lar-ws-chan-client-async boost_chrono)