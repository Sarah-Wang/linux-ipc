cmake_minimum_required(VERSION 2.8)
project(msgq)

include_directories(.)
#LINK_DIRECTORIES(/usr/local/lib)

add_executable(systemv_sem systemv_sem.c)
add_executable(systemv_sem_my systemv_sem_my.c)
target_link_libraries(systemv_sem_my pthread)

add_executable(posix_sem_named posix_sem_named.c)
target_link_libraries(posix_sem_named pthread)

add_executable(posix_sem_unnamed posix_sem_unnamed.c)
target_link_libraries(posix_sem_unnamed pthread)

add_executable(posix_sem_unnamed_my posix_sem_unnamed_my.c)
target_link_libraries(posix_sem_unnamed_my pthread)

