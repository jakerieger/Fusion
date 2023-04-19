cmake_minimum_required(VERSION 3.22.1)
project(infuse C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(infuse
    src/main.c
)