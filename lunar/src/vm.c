// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "vm.h"
#include "io.h"
#include "repl.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

VM* initialize_vm() {
    VM* vm;
    vm = (VM*) malloc(sizeof(VM));
    if (vm == NULL) { return NULL; }

    // Initialize config memory
    vm->config = (VMConfig*) malloc(sizeof(VMConfig));

    // Parse vmconfig if exists, otherwise use defaults and write a new vmconfig
    // TODO: Hard code this path to avoid any file mimic exploits
    const char* config_path = get_platform_config_dir();
    char config_file_path[256];
    snprintf(config_file_path, sizeof(config_file_path), "%s/%s", config_path, ".vmconfig");
    if (load_vm_config(config_file_path, vm->config) != 0) {
        // First check if config directory exists and if not, create it
        struct stat st = {0};
        if (stat(config_path, &st) == -1) {
            // Directory does not exist, create it
            if (mkdir(config_path, 0700) == -1) {
                // Error creating directory
                print_error("Error creating config directory: "
                            "\n  You may not have permission.");
                return NULL;
            }
        }

        // Save to .vmconfig
        FILE* file = fopen(config_file_path, "w");
        int result = fputs(CONFIG_TEMPLATE, file);
        if (result == EOF) {
            print_error("Failed to save .vmconfig\n");
            return NULL;
        }
        fclose(file);
    }

    // Initialize stack & heap
    vm->heap = create_heap(vm->config->heap_size);
    vm->stack = create_stack((int) vm->config->stack_size);

    // Initialize memory ptrs
    vm->stack_ptr = -1;
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    // Initialize symbol table
    vm->symbol_table = malloc(sizeof(HashMap));
    if (vm->symbol_table == NULL) {
        print_error("Failed to allocate memory for symbol table.\n");
        return NULL;
    }
    init_hash_map(vm->symbol_table);

    // Initialize register
    vm->register_u64 = 0;
    vm->register_u32 = 0;

    return vm;
}

int configure_vm(VM* vm) { return 0; }

int shutdown_vm(VM* vm) {
    free_hash_map(vm->symbol_table);
    free(vm->config);
    free(vm->heap);
    free(vm->stack);
    free(vm);
    return 0;
}

int run_program(VM* vm, InstructionStream* stream) {
    //vm->stack_ptr = -1;
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    clock_t begin = clock();

    while (stream->instructions[vm->instr_ptr].opcode != OP_HALT) {
        Instruction instruction = stream->instructions[vm->instr_ptr];
        vm->instr_ptr++;

        switch (instruction.opcode) {
            case OP_PUSH:
                if (instruction.operand_type == OP_TYPE_NUMBER) {
                    push(vm->stack, &instruction.operand.number, 0);
                } else if (instruction.operand_type == OP_TYPE_STR) {
                    push(vm->stack, instruction.operand.symbol, 1);
                }
                break;
            case OP_POP:
                pop(vm->stack, 0);
                break;
            case OP_ADD: {
                double right = *(double*) pop(vm->stack, 0);
                double left = *(double*) pop(vm->stack, 0);
                double value = left + right;
                push(vm->stack, &value, 0);
            } break;
            case OP_SUBTRACT: {
                double right = *(double*) pop(vm->stack, 0);
                double left = *(double*) pop(vm->stack, 0);
                double value = left - right;
                push(vm->stack, &value, 0);
            } break;
            case OP_MULTIPLY: {
                double right = *(double*) pop(vm->stack, 0);
                double left = *(double*) pop(vm->stack, 0);
                double value = left * right;
                push(vm->stack, &value, 0);
            } break;
            case OP_DIVIDE: {
                double right = *(double*) pop(vm->stack, 0);
                double left = *(double*) pop(vm->stack, 0);
                double value = left / right;
                push(vm->stack, &value, 0);
            } break;
            case OP_MODULUS: {
                double right = *(double*) pop(vm->stack, 0);
                double left = *(double*) pop(vm->stack, 0);
                double value = left / right;
                push(vm->stack, &value, 0);
            } break;
            case OP_LOAD: {
                char* symbol_name = instruction.operand.symbol;
                Node* value = get_node(vm->symbol_table, symbol_name);
                if (value == NULL) {
                    printf("[!] Runtime error: Unknown identifier '%s'\n", symbol_name);
                    break;
                }

                if (value->value_type == NVT_NUMBER) {
                    push(vm->stack, &value->value_number, 0);
                    printf("%s: %f\n", symbol_name, value->value_number);
                } else {
                    push(vm->stack, &value->value_str, 1);
                    printf("%s: %s\n", symbol_name, value->value_str);
                }
            } break;
            case OP_STORE: {
                char* symbol_name = instruction.operand.symbol;
                if (instruction.store_type == ST_STR) {
                    char* value = (char*) pop(vm->stack, 1);
                    insert(vm->symbol_table, symbol_name, value, 1);
                } else if (instruction.store_type == ST_NUMBER) {
                    double value = *(double*) pop(vm->stack, 0);
                    insert(vm->symbol_table, symbol_name, &value, 0);
                } else {
                    print_error("Attempted to store reference of unknown type.\n");
                    return -1;
                }
            } break;
                //            case OP_HEAP_ALLOC:
                //                break;
                //            case OP_HEAP_FREE:
                //                break;
                //            case OP_NOOP:
                //                break;
                //            case OP_HALT:
                //                break;
            default:
                break;
        }
    }

    clock_t end = clock();
    double runtime = (double) (end - begin) / CLOCKS_PER_SEC * 1000;
    //    printf("Output (double) => ");
    //    double output_double = *(double*) pop(vm->stack, 0);
    //    printf("%s%f%s\n", BOLD_CYAN_COLOR, output_double, RESET_COLOR);
    printf("%s(took: %.3fms)%s\n", YELLOW_COLOR, runtime, RESET_COLOR);

    if (vm->config->emit_instruction_set == 1) {
        int* byte_code = generate_byte_code(vm, stream);
        printf("\nGenerated Bytecode:\n");
        for (int i = 0; i < stream->count - 1; i++) {
            printf("0x%02X", byte_code[i]);
            if (i < stream->count - 2) { printf(", "); }
            if ((i + 1) % 8 == 0) { printf("\n"); }
        }
        printf("\n");
    }

    return 0;
}

int reset_vm(VM* vm) { return 0; }

int* generate_byte_code(VM* vm, InstructionStream* stream) {
    // Re-initialize VM ptrs
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    int* opcodes = malloc(stream->count * sizeof(int));
    int idx = 0;
    while (stream->instructions[vm->instr_ptr].opcode != OP_HALT) {
        Instruction instruction = stream->instructions[vm->instr_ptr];
        vm->instr_ptr++;
        opcodes[idx] = instruction.opcode;
        idx++;
    }

    return opcodes;
}
