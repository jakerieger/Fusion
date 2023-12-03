cmake_minimum_required(VERSION 3.22.1)
project(fuse-test C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL_SOURCES
    ${PROJECT_SOURCE_DIR}/../fuse/src/repl.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}/../fuse/include
    ${PROJECT_SOURCE_DIR}/../fuse/vendor 
)

add_executable(fuse-test main.c ${EXTERNAL_SOURCES})
target_link_libraries(fuse-test m)
