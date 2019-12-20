cmake_minimum_required(VERSION 2.8)
project(fifo)

include_directories(.)
#LINK_DIRECTORIES(/usr/local/lib)

add_executable(fifo_srv fifo_srv.c)
add_executable(fifo_cli fifo_cli.c)

