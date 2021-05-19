cmake_minimum_required(VERSION 3.1)
project(NeedhamSchroeder)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(SQLite3 REQUIRED)
add_subdirectory(qc)

add_subdirectory(mq_setup)
add_subdirectory(server)
add_subdirectory(client)
