// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "vm.h"
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
    vm->call_stack = create_call_stack(vm->config->call_stack_size * sizeof(CallFrame));

    // Initialize memory ptrs
    vm->stack_ptr = -1;
    vm->call_stack_ptr = -1;
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    // Initialize symbol table
    vm->symbol_table = malloc(sizeof(HashMap));
    if (vm->symbol_table == NULL) {
        print_error("Failed to allocate memory for symbol table.\n");
        return NULL;
    }
    init_hash_map(vm->symbol_table);

    vm->function_table = perf_hash_map_new();

    // Initialize registers
    vm->register_i64 = 0L;
    vm->register_i32 = 0;
    vm->register_i16 = 0;
    vm->register_i8 = 0;

    return vm;
}

int configure_vm(VM* vm) { return 0; }

int shutdown_vm(VM* vm) {
    free_hash_map(vm->symbol_table);
    perf_hash_map_free(vm->function_table);
    free(vm->config);
    free(vm->heap);
    free(vm->stack);
    free(vm);
    return 0;
}

int run_program(VM* vm, InstructionStream* stream) {
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
                        push(vm->stack, instruction.operand.string_value, LUNA_TYPE_STRING);
                        break;
                    case OP_TYPE_NUMBER:
                        push(vm->stack, &instruction.operand.number_value, LUNA_TYPE_NUMBER);
                        break;
                    case OP_TYPE_BOOLEAN:
                        push(vm->stack, &instruction.operand.boolean_value, LUNA_TYPE_BOOLEAN);
                        break;
                    case OP_TYPE_SYMBOL:
                        push(vm->stack, instruction.operand.symbol, LUNA_TYPE_STRING);
                        break;
                    case OP_TYPE_NULL:
                        break;
                }
                break;
            case OP_POP:
                pop(vm->stack, NULL);
                break;
            case OP_ADD: {
                LunaNumber right = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber left = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber value = left + right;
                push(vm->stack, &value, LUNA_TYPE_NUMBER);
            } break;
            case OP_SUBTRACT: {
                LunaNumber right = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber left = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber value = left - right;
                push(vm->stack, &value, LUNA_TYPE_NUMBER);
            } break;
            case OP_MULTIPLY: {
                LunaNumber right = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber left = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber value = left * right;
                push(vm->stack, &value, LUNA_TYPE_NUMBER);
            } break;
            case OP_DIVIDE: {
                LunaNumber right = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber left = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber value = left / right;
                push(vm->stack, &value, LUNA_TYPE_NUMBER);
            } break;
            case OP_MODULUS: {
                LunaNumber right = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber left = *(LunaNumber*) pop(vm->stack, NULL);
                LunaNumber value = fmod(left, right);
                push(vm->stack, &value, LUNA_TYPE_NUMBER);
            } break;
            case OP_LOAD: {
                char* symbol_name = instruction.operand.symbol;
                Node* value = get_node(vm->symbol_table, symbol_name);
                if (value == NULL) {
                    printf("[!] Runtime error: Unknown identifier '%s'\n", symbol_name);
                    break;
                }

                switch (value->value_type) {
                    case LUNA_TYPE_NUMBER:
                        push(vm->stack, &value->as.number_value, LUNA_TYPE_NUMBER);
                        break;
                    case LUNA_TYPE_STRING:
                        push(vm->stack, value->as.string_value, LUNA_TYPE_STRING);
                        break;
                    case LUNA_TYPE_BOOLEAN:
                        push(vm->stack, &value->as.boolean_value, LUNA_TYPE_BOOLEAN);
                        break;
                    case LUNA_TYPE_NULL:
                    default:
                        print_error("Symbol '%s' is type 'null'\n", symbol_name);
                        break;
                }
            } break;
            case OP_STORE: {
                char* symbol_name = instruction.operand.symbol;

                // We already know the type from the element on the stack.
                // If we adjust the pop method to push a type out to a
                // variable we provide, we can handle casting without
                // needing to pass a StoreType enum to this OpCode.
                LunaType* type = malloc(sizeof(LunaType));
                void* value = pop(vm->stack, type);

                if (type == NULL) {
                    print_error("LunaType pointer value NULL\n");
                    break;
                }

                switch (*type) {
                    case LUNA_TYPE_STRING: {
                        LunaString typed_value = (LunaString) value;
                        insert(vm->symbol_table, symbol_name, typed_value, LUNA_TYPE_STRING);
                    } break;
                    case LUNA_TYPE_NUMBER: {
                        LunaNumber typed_value = *(LunaNumber*) value;
                        insert(vm->symbol_table, symbol_name, &typed_value, LUNA_TYPE_NUMBER);
                    } break;
                    case LUNA_TYPE_BOOLEAN: {
                        LunaBoolean typed_value = *(LunaBoolean*) value;
                        insert(vm->symbol_table, symbol_name, &typed_value, LUNA_TYPE_BOOLEAN);
                    } break;
                    default:
                        print_error("Attempted to store a null reference.\n");
                        return -1;
                }

                free(type);
            } break;
            case OP_PRINT: {
                LunaType* type = malloc(sizeof(LunaType));
                *type = LUNA_TYPE_NULL;
                if (type == NULL) {
                    print_error("Failed to allocate memory for output\n");
                    return 1;
                }
                void* output = pop(vm->stack, type);
                printf("=> ");
                switch (*type) {
                    case LUNA_TYPE_BOOLEAN: {
                        LunaBoolean typed_output = *(LunaBoolean*) output;
                        printf("%s%s%s\n", BOLD_CYAN_COLOR,
                               typed_output.value == BOOL_TRUE ? "true" : "false", RESET_COLOR);
                    } break;
                    case LUNA_TYPE_NUMBER: {
                        LunaNumber typed_output = *(LunaNumber*) output;
                        printf("%s%f%s\n", BOLD_CYAN_COLOR, typed_output, RESET_COLOR);
                    } break;
                    case LUNA_TYPE_STRING: {
                        LunaString typed_output = (LunaString) output;
                        printf("%s\"%s\"%s\n", BOLD_CYAN_COLOR, typed_output, RESET_COLOR);
                    } break;
                    case LUNA_TYPE_NULL:
                        printf("%s%s%s\n", BOLD_YELLOW_COLOR, "null", RESET_COLOR);
                        break;
                }
            } break;
            case OP_CALL: {
            } break;
            case OP_NEW_FUNC: {
                char* func_name = strdup(instruction.operand.function_object.name);
                perf_hash_map_insert(vm->function_table, func_name,
                                     (FunctionObject) instruction.operand.function_object);
            } break;
            case OP_NOOP:
                continue;
                //            case OP_HEAP_ALLOC:
                //                break;
                //            case OP_HEAP_FREE:
                //                break;
                //            case OP_HALT:
                //                break;
            default:
                break;
        }
    }

    clock_t end = clock();
    double runtime = (double) (end - begin) / CLOCKS_PER_SEC * 1000;
    printf("%s(took: %.3fms)%s\n", YELLOW_COLOR, runtime, RESET_COLOR);

    if (vm->config->emit_instruction_set == 1) {
        char** asm_code = generate_vm_assembly(vm, stream);
        printf("\nGenerated VM Assembly:\n");

        for (int i = 0; *(asm_code + i); i++) {
            print_colored(BOLD_WHITE_COLOR, "%s\n", *(asm_code + i));
        }

        printf("\n");
    }

    pop(vm->stack, NULL);
    return 0;
}

int reset_vm(VM* vm) { return 0; }

char** generate_vm_assembly(VM* vm, InstructionStream* stream) {
    // Re-initialize VM ptrs
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    char** asm_array = NULL;
    int size = 0;
    asm_array = malloc(sizeof(char*) * 1);
    if (asm_array == NULL) {
        print_error("Unable to allocate memory for vm assembly code.\n");
        exit(1);
    }

    int idx = 0;
    while (stream->instructions[vm->instr_ptr].opcode != OP_HALT) {
        Instruction instruction = stream->instructions[vm->instr_ptr];
        vm->instr_ptr++;

        switch (instruction.opcode) {
            case OP_PUSH: {
                char push_str[] = "PUSH     $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_LOAD: {
                char push_str[] = "MOV      $SYM $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_POP: {
                char push_str[] = "POP      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_STORE: {
                char push_str[] = "MOV      $SP $SYM";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_ADD: {
                char push_str[] = "ADD      $SP, $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_SUBTRACT: {
                char push_str[] = "SUB      $SP, $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_MULTIPLY: {
                char push_str[] = "MUL      $SP, $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_DIVIDE: {
                char push_str[] = "DIV      $SP, $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_MODULUS: {
                char push_str[] = "MOD      $SP, $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_PRINT: {
                char push_str[] = "__print  $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_INPUT: {
                char push_str[] = "__input  $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_HEAP_ALLOC: {
                char push_str[] = "HAL      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_HEAP_FREE: {
                char push_str[] = "HFR      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_HALT: {
                char push_str[] = "HALT";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_NOOP: {
                char push_str[] = "NOOP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_JUMP: {
                char push_str[] = "JMP      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_JUMP_IF_TRUE: {
                char push_str[] = "JIT      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_JUMP_IF_FALSE: {
                char push_str[] = "JIF      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_EQ: {
                char push_str[] = "CEQ      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_NE: {
                char push_str[] = "CNE      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_LT: {
                char push_str[] = "CLT      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_GT: {
                char push_str[] = "CGT      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_LE: {
                char push_str[] = "CLE      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_COMPARE_GE: {
                char push_str[] = "CGE      $SP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
        }

        idx++;
    }
    pop(vm->stack, NULL);
    return asm_array;
}