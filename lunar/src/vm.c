// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "vm.h"
#include "io.h"
#include "repl.h"
#include <math.h>
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
                switch (instruction.operand_type) {
                    case OP_TYPE_STRING:
                        push(vm->stack, instruction.operand.string_value, FT_STRING);
                        break;
                    case OP_TYPE_NUMBER:
                        push(vm->stack, &instruction.operand.number_value, FT_NUMBER);
                        break;
                    case OP_TYPE_BOOLEAN:
                        push(vm->stack, &instruction.operand.boolean_value, FT_BOOLEAN);
                        break;
                    case OP_TYPE_SYMBOL:
                        push(vm->stack, instruction.operand.symbol, FT_STRING);
                        break;
                    case OP_TYPE_NULL:
                        break;
                }
                break;
            case OP_POP:
                pop(vm->stack, NULL);
                break;
            case OP_ADD: {
                Number right = *(Number*) pop(vm->stack, NULL);
                Number left = *(Number*) pop(vm->stack, NULL);
                Number value = left + right;
                push(vm->stack, &value, FT_NUMBER);
            } break;
            case OP_SUBTRACT: {
                Number right = *(Number*) pop(vm->stack, NULL);
                Number left = *(Number*) pop(vm->stack, NULL);
                Number value = left - right;
                push(vm->stack, &value, FT_NUMBER);
            } break;
            case OP_MULTIPLY: {
                Number right = *(Number*) pop(vm->stack, NULL);
                Number left = *(Number*) pop(vm->stack, NULL);
                Number value = left * right;
                push(vm->stack, &value, FT_NUMBER);
            } break;
            case OP_DIVIDE: {
                Number right = *(Number*) pop(vm->stack, NULL);
                Number left = *(Number*) pop(vm->stack, NULL);
                Number value = left / right;
                push(vm->stack, &value, FT_NUMBER);
            } break;
            case OP_MODULUS: {
                Number right = *(Number*) pop(vm->stack, NULL);
                Number left = *(Number*) pop(vm->stack, NULL);
                Number value = fmod(left, right);
                push(vm->stack, &value, FT_NUMBER);
            } break;
            case OP_LOAD: {
                char* symbol_name = instruction.operand.symbol;
                Node* value = get_node(vm->symbol_table, symbol_name);
                if (value == NULL) {
                    printf("[!] Runtime error: Unknown identifier '%s'\n", symbol_name);
                    break;
                }

                switch (value->value_type) {
                    case NVT_NUMBER:
                        push(vm->stack, &value->as.number_value, FT_NUMBER);
                        printf("%s: %f\n", symbol_name, value->as.number_value);
                        break;
                    case NVT_STRING:
                        push(vm->stack, &value->as.string_value, FT_STRING);
                        printf("%s: %s\n", symbol_name, value->as.string_value);
                        break;
                    case NVT_BOOLEAN:
                        push(vm->stack, &value->as.boolean_value, FT_BOOLEAN);
                        printf("%s: %s\n", symbol_name,
                               value->as.boolean_value.value == BOOL_TRUE ? "true" : "false");
                        break;
                }
            } break;
            case OP_STORE: {
                char* symbol_name = instruction.operand.symbol;

                // We already know the type from the element on the stack.
                // If we adjust the pop method to push a type out to a
                // variable we provide, we can handle casting without
                // needing to pass a StoreType enum to this OpCode.
                FrameType* type = malloc(sizeof(FrameType));
                void* value = pop(vm->stack, type);

                if (type == NULL) {
                    print_error("FrameType pointer value NULL\n");
                    break;
                }

                switch (*type) {
                    case FT_STRING: {
                        String typed_value = (String) value;
                        insert(vm->symbol_table, symbol_name, &typed_value, NVT_STRING);
                    } break;
                    case FT_NUMBER: {
                        Number typed_value = *(Number*) value;
                        insert(vm->symbol_table, symbol_name, &typed_value, NVT_NUMBER);
                    } break;
                    case FT_BOOLEAN: {
                        Boolean typed_value = *(Boolean*) value;
                        insert(vm->symbol_table, symbol_name, &typed_value, NVT_BOOLEAN);
                    } break;
                    default:
                        print_error("Attempted to store a null reference.\n");
                        return -1;
                }

                free(type);
            } break;
                //            case OP_HEAP_ALLOC:
                //                break;
                //            case OP_HEAP_FREE:
                //                break;
            case OP_NOOP:
                continue;
                //            case OP_HALT:
                //                break;
            default:
                break;
        }
    }

    clock_t end = clock();
    Number runtime = (Number) (end - begin) / CLOCKS_PER_SEC * 1000;
    //    printf("Output (Number) => ");
    //    Number output_Number = *(Number*) pop(vm->stack, 0);
    //    printf("%s%f%s\n", BOLD_CYAN_COLOR, output_Number, RESET_COLOR);
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
