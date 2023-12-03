// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#include "vmconfig.h"
#include "inih/ini.h"
#include <stdlib.h>
#include <string.h>

int parse_vm_config(void* config, const char* section, const char* name, const char* value) {
    VMConfig* vm_config = (VMConfig*) config;
    // Initialize struct values to defaults before attempting to read the config
    vm_config->heap_size = DEFAULT_HEAP_SIZE;
    vm_config->stack_size = DEFAULT_STACK_SIZE;
    vm_config->call_stack_size = DEFAULT_CALL_STACK_SIZE;
    vm_config->num_i64_registers = DEFAULT_NUM_I64_REGISTERS;
    vm_config->num_i32_registers = DEFAULT_NUM_I32_REGISTERS;
    vm_config->num_i16_registers = DEFAULT_NUM_I16_REGISTERS;
    vm_config->num_i8_registers = DEFAULT_NUM_I8_REGISTERS;
    vm_config->num_str_registers = DEFAULT_NUM_STR_REGISTERS;
    vm_config->str_lit_max_len = DEFAULT_STR_LIT_MAX_LEN;
    vm_config->emit_instruction_set = DEFAULT_EMIT_INSTR_SET;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    // TODO: Replace `atoi` calls with `strtol`
    if (MATCH("VIRTUAL_MACHINE", "HEAP_SIZE")) {
        vm_config->heap_size = atoi(value) * 1000;
    } else if (MATCH("VIRTUAL_MACHINE", "STACK_SIZE")) {
        vm_config->stack_size = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "CALL_STACK_SIZE")) {
        vm_config->call_stack_size = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "INT64_REGISTERS")) {
        vm_config->num_i64_registers = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "INT32_REGISTERS")) {
        vm_config->num_i32_registers = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "INT16_REGISTERS")) {
        vm_config->num_i16_registers = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "INT8_REGISTERS")) {
        vm_config->num_i8_registers = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "STR_REGISTERS")) {
        vm_config->num_str_registers = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "STRING_LIT_MAX_LEN")) {
        vm_config->str_lit_max_len = atoi(value);
    } else if (MATCH("VIRTUAL_MACHINE", "EMIT_INSTRUCTION_SET")) {
        vm_config->emit_instruction_set = atoi(value);
    } else {
        return 0;
    }

    return 1;
}

int load_vm_config(const char* restrict config_path, VMConfig* vm_config) {
    if (ini_parse(config_path, parse_vm_config, vm_config) < 0) { return 1; }

    return 0;
}
