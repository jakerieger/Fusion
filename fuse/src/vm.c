// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "vm.h"
#include "error.h"
#include "hashmap.h"
#include "instruction.h"
#include "repl.h"
#include "stack.h"
#include "symbol_table.h"
#include "types.h"
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
    vm->call_stack_ptr = -1;
    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    // Initialize symbol table stack
    vm->symbol_table_stack = malloc(sizeof(SymbolTableStack));
    CHECK_MALLOC(vm->symbol_table_stack, "vm->symbol_table_stack")
    HashMap* global_symbol_table = hashmap_create();
    vm->symbol_table_stack->tables = malloc(sizeof(HashMap));
    vm->symbol_table_stack->tables[0] = global_symbol_table;
    vm->symbol_table_stack->count = 1;

    vm->function_table = hashmap_create(vm->function_table);

    // Initialize registers
    vm->register_i64 = 0L;
    vm->register_i32 = 0;
    vm->register_i16 = 0;
    vm->register_i8 = 0;

    return vm;
}

int configure_vm(VM* vm) { return 0; }

int shutdown_vm(VM* vm) {
    for (int i = 0; i < vm->symbol_table_stack->count; i++) {
        hashmap_destroy(vm->symbol_table_stack->tables[i]);
    }

    hashmap_destroy(vm->function_table);
    free(vm->config);
    free(vm->heap);
    free(vm->stack);
    free(vm);
    return 0;
}

int run_program(VM* vm, InstructionStream* stream, const char* proc_name) {
#define SAFE_TERMINATE vm->instr_ptr = stream->count - 1;

    vm->heap_ptr = 0;
    vm->instr_ptr = 0;

    while (stream->instructions[vm->instr_ptr].opcode != OP_HALT) {
        Instruction instruction = stream->instructions[vm->instr_ptr];
        vm->instr_ptr++;

        switch (instruction.opcode) {
            case OP_PUSH:
                switch (instruction.operand_context.type) {
                    case OP_TYPE_STRING:
                        push(vm->stack, instruction.operand.string_value, FUSION_TYPE_STRING);
                        break;
                    case OP_TYPE_NUMBER:
                        push(vm->stack, &instruction.operand.number_value, FUSION_TYPE_NUMBER);
                        break;
                    case OP_TYPE_BOOLEAN:
                        push(vm->stack, &instruction.operand.boolean_value, FUSION_TYPE_BOOLEAN);
                        break;
                    case OP_TYPE_SYMBOL:
                        push(vm->stack, instruction.operand.symbol, FUSION_TYPE_STRING);
                        break;
                    case OP_TYPE_NULL:
                    case OP_TYPE_FUNCTION:
                    default:
                        break;
                }
                break;
            case OP_POP:
                pop(vm->stack, NULL);
                break;
            case OP_ADD: {
                FusionNumber right = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber left = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber value = left + right;
                push(vm->stack, &value, FUSION_TYPE_NUMBER);
            } break;
            case OP_SUBTRACT: {
                FusionNumber right = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber left = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber value = left - right;
                push(vm->stack, &value, FUSION_TYPE_NUMBER);
            } break;
            case OP_MULTIPLY: {
                FusionNumber right = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber left = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber value = left * right;
                push(vm->stack, &value, FUSION_TYPE_NUMBER);
            } break;
            case OP_DIVIDE: {
                FusionNumber right = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber left = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber value = left / right;
                push(vm->stack, &value, FUSION_TYPE_NUMBER);
            } break;
            case OP_MODULUS: {
                FusionNumber right = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber left = *(FusionNumber*) pop(vm->stack, NULL);
                FusionNumber value = fmod(left, right);
                push(vm->stack, &value, FUSION_TYPE_NUMBER);
            } break;
            case OP_LOAD: {
                char* symbol_name = instruction.operand.symbol;
                Entry* entry = hashmap_get(
                        instruction.operand_context.scope == SCOPE_GLOBAL
                                ? vm->symbol_table_stack->tables[0]
                                : vm->symbol_table_stack->tables[vm->symbol_table_stack->count - 1],
                        symbol_name);
                if (entry == NULL) {
                    print_error("Undefined variable '%s'", symbol_name);
                    SAFE_TERMINATE
                    continue;
                }

                switch (entry->value_type) {
                    case MAP_TYPE_STRING:
                        push(vm->stack, entry->as.string_value, FUSION_TYPE_STRING);
                        break;
                    case MAP_TYPE_NUMBER:
                        push(vm->stack, &entry->as.number_value, FUSION_TYPE_NUMBER);
                        break;
                    case MAP_TYPE_BOOLEAN:
                        push(vm->stack, &entry->as.bool_value, FUSION_TYPE_BOOLEAN);
                        break;
                    case MAP_TYPE_FUNCTION:
                        push(vm->stack, &entry->as.function_value.name, FUSION_TYPE_STRING);
                        break;
                    case MAP_TYPE_NULL:
                    default:
                        break;
                }
            } break;
            case OP_STORE: {
                char* symbol_name = instruction.operand.symbol;

                // We already know the type from the element on the stack.
                // If we adjust the pop method to push a type out to a
                // variable we provide, we can handle casting without
                // needing to pass a StoreType enum to this OpCode.
                FusionType* type = malloc(sizeof(FusionType));
                void* value = pop(vm->stack, type);

                if (type == NULL) {
                    print_error("FusionType pointer value NULL\n");
                    SAFE_TERMINATE
                    continue;
                }

                switch (*type) {
                    case FUSION_TYPE_STRING: {
                        FusionString typed_value = (FusionString) value;
                        hashmap_put(instruction.operand_context.scope == SCOPE_GLOBAL
                                            ? vm->symbol_table_stack->tables[0]
                                            : vm->symbol_table_stack
                                                      ->tables[vm->symbol_table_stack->count - 1],
                                    symbol_name, typed_value, MAP_TYPE_STRING);
                    } break;
                    case FUSION_TYPE_NUMBER: {
                        FusionNumber typed_value = *(FusionNumber*) value;
                        hashmap_put(instruction.operand_context.scope == SCOPE_GLOBAL
                                            ? vm->symbol_table_stack->tables[0]
                                            : vm->symbol_table_stack
                                                      ->tables[vm->symbol_table_stack->count - 1],
                                    symbol_name, &typed_value, MAP_TYPE_NUMBER);
                    } break;
                    case FUSION_TYPE_BOOLEAN: {
                        FusionBoolean typed_value = *(FusionBoolean*) value;
                        hashmap_put(instruction.operand_context.scope == SCOPE_GLOBAL
                                            ? vm->symbol_table_stack->tables[0]
                                            : vm->symbol_table_stack
                                                      ->tables[vm->symbol_table_stack->count - 1],
                                    symbol_name, &typed_value, MAP_TYPE_BOOLEAN);
                    } break;
                    default:
                        print_error("Attempted to store a null reference.\n");
                        SAFE_TERMINATE
                        continue;
                }

                free(type);
            } break;
            case OP_PRINT: {
                FusionType* type = malloc(sizeof(FusionType));
                *type = FUSION_TYPE_NULL;
                if (type == NULL) {
                    print_error("Failed to allocate memory for output\n");
                    SAFE_TERMINATE
                    continue;
                }
                void* output = pop(vm->stack, type);
                switch (*type) {
                    case FUSION_TYPE_BOOLEAN: {
                        printf("\n(Boolean) => ");
                        FusionBoolean typed_output = *(FusionBoolean*) output;
                        printf("%s%s%s\n", BOLD_CYAN_COLOR,
                               typed_output.value == BOOL_TRUE ? "true" : "false", RESET_COLOR);
                    } break;
                    case FUSION_TYPE_NUMBER: {
                        printf("\n(Number) => ");
                        FusionNumber typed_output = *(FusionNumber*) output;
                        printf("%s%f%s\n", BOLD_CYAN_COLOR, typed_output, RESET_COLOR);
                    } break;
                    case FUSION_TYPE_STRING: {
                        printf("\n(String) => ");
                        FusionString typed_output = (FusionString) output;
                        printf("%s%s%s\n", BOLD_CYAN_COLOR, typed_output, RESET_COLOR);
                    } break;
                    case FUSION_TYPE_NULL:
                        printf("\n(Undefined) => ");
                        printf("%s%s%s\n", BOLD_YELLOW_COLOR, "null", RESET_COLOR);
                        break;
                }
            } break;
            case OP_CALL: {
                char* func_name = instruction.operand.symbol;

                Entry* func_entry = hashmap_get(vm->function_table, func_name);
                if (func_entry == NULL) {
                    print_error("Tried to call undefined function: '%s'.", func_name);
                    SAFE_TERMINATE
                    continue;
                }

                if (func_entry->value_type != MAP_TYPE_FUNCTION) {
                    print_error("Tried to call undefined reference '%s' as function.", func_name);
                    SAFE_TERMINATE
                    continue;
                }

                // This method works fine when all the arguments are position-independent
                // TODO: Refactor this to retrieve values in the order they're provided to
                //       the function call
                FunctionObject* func_obj = &func_entry->as.function_value;
                for (int i = func_obj->argc - 1; i >= 0; i--) {
                    char* symbol = func_obj->args_names[i];
                    FusionType* type = malloc(sizeof(FusionType));
                    *type = FUSION_TYPE_NULL;
                    void* value = pop(vm->stack, type);
                    switch (*type) {
                        case FUSION_TYPE_BOOLEAN: {
                            FusionBoolean typed_output = *(FusionBoolean*) value;
                            hashmap_put(vm->symbol_table_stack->tables[0], symbol, &typed_output,
                                        MAP_TYPE_BOOLEAN);
                        } break;
                        case FUSION_TYPE_NUMBER: {
                            FusionNumber typed_output = *(FusionNumber*) value;
                            hashmap_put(vm->symbol_table_stack->tables[0], symbol, &typed_output,
                                        MAP_TYPE_NUMBER);
                        } break;
                        case FUSION_TYPE_STRING: {
                            FusionString typed_output = (FusionString) value;
                            hashmap_put(vm->symbol_table_stack->tables[0], symbol, typed_output,
                                        MAP_TYPE_STRING);
                        } break;
                        case FUSION_TYPE_NULL:
                            break;
                    }

                    free(type);
                }

                int return_addr = vm->instr_ptr;
                run_program(vm, func_obj->body, func_name);
                vm->instr_ptr = return_addr;
                continue;

            } break;
            case OP_NEW_FUNC: {
                char* name = strdup(instruction.operand.function_object.name);
                hashmap_put(vm->function_table, name,
                            (FunctionObject*) &instruction.operand.function_object,
                            MAP_TYPE_FUNCTION);
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

    if (vm->config->emit_instruction_set == 1) {
        char** asm_code = generate_vm_assembly(vm, stream, proc_name);
        printf("\nGenerated VM Assembly:\n");

        for (int i = 0; *(asm_code + i); i++) {
            print_colored(BOLD_WHITE_COLOR, "%s\n", *(asm_code + i));
        }

        printf("\n");
    }

    pop(vm->stack, NULL);

#undef SAFE_TERMINATE
    return 0;
}

int reset_vm(VM* vm) { return 0; }

char** generate_vm_assembly(VM* vm, InstructionStream* stream, const char* proc_name) {
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
            case OP_CALL: {
                char* name = instruction.operand.symbol;
                char push_str[24];
                sprintf(push_str, "__%s()", name);
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            }
            case OP_RETURN:
            case OP_LOAD_PARAM:
            case OP_STORE_PARAM:
            case OP_LOAD_LOCAL:
            case OP_STORE_LOCAL:
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_NEW_FUNC: {
                char push_str[] = "DEF      $FP";
                asm_array[size] = malloc(strlen(push_str) + 1);
                strcpy(asm_array[size], push_str);
                size++;
                asm_array = realloc(asm_array, sizeof(char*) * (size + 1));
            } break;
            case OP_LOAD_FUNC:
            case OP_CLOSE_FUNC:
                break;
        }

        idx++;
    }
    pop(vm->stack, NULL);
    return asm_array;
}