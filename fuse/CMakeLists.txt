cmake_minimum_required(VERSION 3.22.1)
project(fuse C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    include
    vendor
)

add_executable(fuse
    include/ast.h
    include/call_stack.h
    include/error.h
    include/fuse.h
    include/hashmap.h
    include/heap.h
    include/instruction.h
    include/io.h
    include/lexer.h
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
    src/hashmap.c
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
        std/crypto/djb2.h)

target_link_libraries(fuse m)