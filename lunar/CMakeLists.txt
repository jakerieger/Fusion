cmake_minimum_required(VERSION 3.22.1)
project(lunar C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    include
    vendor
)

add_executable(lunar
    include/ast.h
    include/call_stack.h
    include/error.h
    include/hash_map.h
    include/heap.h
    include/instruction.h
    include/io.h
    include/lexer.h
    include/lunar.h
    include/platform.h
    include/register.h
    include/repl.h
    include/stack.h
    include/std.h
    include/token_stream.h
    include/types.h
    include/vm.h
    include/vmconfig.h
    src/ast.c
    src/call_stack.c
    src/hash_map.c
    src/heap.c
    src/instruction.c
    src/io.c
    src/lexer.c
    src/main.c
    src/register.c
    src/repl.c
    src/stack.c
    src/std.c
    src/token_stream.c
    src/vm.c
    src/vmconfig.c
    vendor/inih/ini.c
    )

add_custom_command(TARGET lunar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../examples/ ${CMAKE_CURRENT_BINARY_DIR}/examples/)

target_link_libraries(lunar m)