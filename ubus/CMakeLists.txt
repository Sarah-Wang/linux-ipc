cmake_minimum_required(VERSION 2.8)
project(ubus)

include_directories(.)
#LINK_DIRECTORIES(/usr/local/lib)


add_executable(ubus_client ubus_client.c )
target_link_libraries(ubus_client ubus ubox blobmsg_json json-c)

add_executable(ubus_server ubus_server.c )
target_link_libraries(ubus_server ubus ubox blobmsg_json json-c)

#install(TARGETS ipc RUNTIME DESTINATION usr/bin)

