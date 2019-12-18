cmake_minimum_required(VERSION 2.8)
project(msgq)

include_directories(.)
#LINK_DIRECTORIES(/usr/local/lib)

add_executable(systemv_msg_server systemv_msg_server.c)


add_executable(systemv_msg_client systemv_msg_client.c)

add_executable(posix_msg_server posix_msg_server.c)
target_link_libraries(posix_msg_server rt)

add_executable(posix_msg_client posix_msg_client.c)
target_link_libraries(posix_msg_client rt)
