cmake_minimum_required(VERSION 2.8)
project(shm)

include_directories(.)
#LINK_DIRECTORIES(/usr/local/lib)

add_executable(systemv_client shm_systemv_cli.c common.c)
add_executable(systemv_spooler shm_systemv_srv.c common.c)

add_executable(posix_client shm_posix_cli.c common.c)
target_link_libraries(posix_client rt pthread)
add_executable(posix_logger shm_posix_srv.c common.c)
target_link_libraries(posix_logger rt pthread)

