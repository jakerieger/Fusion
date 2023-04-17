// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_VM_CONFIG_H
#define FUSION_VM_CONFIG_H

#define DEFAULT_HEAP_SIZE 512000
#define DEFAULT_STACK_SIZE 512
#define DEFAULT_CALL_STACK_SIZE 256
#define DEFAULT_NUM_I64_REGISTERS 1
#define DEFAULT_NUM_I32_REGISTERS 1
#define DEFAULT_NUM_I16_REGISTERS 1
#define DEFAULT_NUM_I8_REGISTERS 1
#define DEFAULT_NUM_STR_REGISTERS 1
#define DEFAULT_INPUT_BUFFER_SIZE 256
#define DEFAULT_STR_LIT_MAX_LEN 64
#define DEFAULT_EMIT_INSTR_SET 0

#define CONFIG_TEMPLATE                                                                            \
    "#==================================================================#\n"                       \
    "#                Fusion Virtual Machine Configuration                #\n"                       \
    "#==================================================================#\n"                       \
    "\n"                                                                                           \
    "[VIRTUAL_MACHINE]\n"                                                                          \
    "#==================================================================#\n"                       \
    "#                               Size                               #\n"                       \
    "#==================================================================#\n"                       \
    "# These options set the desired sizes the VM will allocate to each #\n"                       \
    "# memory region.                                                   #\n"                       \
    "#==================================================================#\n"                       \
    "# Specify size in Megabytes (MB)\n"                                                           \
    "HEAP_SIZE = 512\n"                                                                            \
    "\n"                                                                                           \
    "# Specify size in number of elements\n"                                                       \
    "STACK_SIZE = 512\n"                                                                           \
    "\n"                                                                                           \
    "# Specify size in number of elements\n"                                                       \
    "CALL_STACK_SIZE = 256\n"                                                                      \
    "\n"                                                                                           \
    "# Optional, size in bytes\n"                                                                  \
    "#INPUT_BUFFER_SIZE = 256\n"                                                                   \
    "\n"                                                                                           \
    "#==================================================================#\n"                       \
    "#                            Registers                             #\n"                       \
    "#==================================================================#\n"                       \
    "# These options set how many of each type of register is created.  #\n"                       \
    "# By default, one of each is created but none are required.        #\n"                       \
    "#==================================================================#\n"                       \
    "INT64_REGISTERS = 1\n"                                                                        \
    "INT32_REGISTERS = 1\n"                                                                        \
    "INT16_REGISTERS = 1\n"                                                                        \
    "INT8_REGISTERS = 1\n"                                                                         \
    "STR_REGISTERS = 1\n"                                                                          \
    "\n"                                                                                           \
    "#==================================================================#\n"                       \
    "#                            REPL Flags                            #\n"                       \
    "#==================================================================#\n"                       \
    "# These options set some configuration flags for the REPL. Mostly  #\n"                       \
    "# useful for debugging purposes and can be omitted entirely.       #\n"                       \
    "#==================================================================#\n"                       \
    "#STRING_LIT_MAX_LEN = 64\n"                                                                   \
    "EMIT_INSTRUCTION_SET = 1"

#include "platform.h"

typedef struct {
    unsigned int heap_size;
    unsigned int stack_size;
    unsigned int call_stack_size;
    unsigned int num_i64_registers;
    unsigned int num_i32_registers;
    unsigned int num_i16_registers;
    unsigned int num_i8_registers;
    unsigned int num_str_registers;
    unsigned int input_buffer_size;
    unsigned int str_lit_max_len;
    unsigned int emit_instruction_set;
} VMConfig;

static int parse_vm_config(void* config, const char* section, const char* name, const char* value);
int load_vm_config(const char* restrict config_path, VMConfig* vm_config);

#endif