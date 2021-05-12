cmake_minimum_required(VERSION 3.1)
project(NeedhamSchroeder)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(qc)
add_executable(main main.c)
target_link_libraries(main PUBLIC qc)
