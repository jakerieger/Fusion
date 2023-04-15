// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "ast.h"
#include "io.h"
#include "lexer.h"
#include "lunar.h"
#include "platform.h"
#include "repl.h"
#include "token_stream.h"
#include "vm.h"
#include <stdio.h>
#include <string.h>

int run_repl(VM* vm, char* input);

/**
 * @brief This function essentially replicates the process used by the REPL,
 * instead substituting typed input for input read from a file.
 * @param path
 */
int execute_script(char* path) {
    char* code = read_file(path);

    VM* vm = initialize_vm();
    if (vm == NULL) {
        print_error("Failed to initialize Virtual Machine.\n");
        exit(1);
    }

    run_repl(vm, code);

    shutdown_vm(vm);

    return 0;
}

/*******************************************************************************
 * We begin by running our sanitized input through our Lexer
 *
 * The lexing process looks like this:
 *  1. Initialize the scanner with our input
 *  2. Create a TokenStream object to store our lexed tokens
 *  3. Run our input through the lexer
 ******************************************************************************/
int run_repl(VM* vm, char* input) {
    init_scanner(input);

    Token token = scan_token();
    int count = 0;
    TokenStream* tokens = malloc(sizeof(TokenStream));
    if (tokens == NULL) {
        print_error("Failed to allocate memory for token stream object.\n");
        return 1;
    }
    tokens->token_ptr = 0;
    tokens->tokens = malloc(sizeof(Token));
    if (tokens->tokens == NULL) {
        print_error("Failed to allocate memory for token stream array.\n");
        return 1;
    }

    while (token.type != TOKEN_EOF) {
        count++;
        tokens->tokens = realloc(tokens->tokens, count * sizeof(Token));

        if (tokens->tokens == NULL) {
            print_error("Memory re-allocation for token stream array failed.\n");
            return 1;
        }

        tokens->tokens[count - 1] = token;
        token = scan_token();
    }

    tokens->count = count;

    /*******************************************************************************
     * Next we pass our TokenStream to our AST generator which will recursively
     * parse our tokens in to a tree of expression nodes that we can later pass off
     * to our instruction parser to generate our machine instructions.
     ******************************************************************************/
    ExprNode* ast = parse_expr(tokens);

    /*******************************************************************************
     * Now we parse our AST in to an InstructionStream we can give to our VM
     * to execute
     ******************************************************************************/
    InstructionStream* program = malloc(sizeof(InstructionStream));
    if (program == NULL) {
        print_error("Failed to allocate memory for InstructionStream.\n");
        exit(1);
    }
    program->count = 0;
    program->instructions = malloc(sizeof(Instruction));
    if (program->instructions == NULL) {
        print_error("Failed to allocate memory for InstructionStream array.\n");
        exit(1);
    }

    generate_instructions(ast, program);
    // We attach the halt instruction to the end of our program to ensure it terminates
    emit_instruction(program, OP_HALT, NULL, OP_TYPE_NULL, ST_NULL);

    /*******************************************************************************
     * We pass our instruction set and VM instance to be executed
     ******************************************************************************/
    run_program(vm, program);

    /*******************************************************************************
     * Finally, we clean up and prepare our VM to receive a new program
     ******************************************************************************/
    cleanup_token_stream(tokens);
    free(program->instructions);
    free(program);
    reset_vm(vm);

    return 0;
}

int main(int argc, char** argv) {
    if (argc > 2 || argc < 1) return -1;

    // File provided, try to execute it
    if (argc == 2) { return execute_script(argv[1]); }

    char input[512];
    char exit_command[] = "exit";
    char info_command[] = "info";
    char config_command[] = "config";

    /*******************************************************************************
     * Print the REPL header text. Unnecessary, but adds a nice visual touch ;]
     ******************************************************************************/
    if (UTF8_SUPPORT == 1) print_colored(BOLD_BLUE_COLOR, (const char*) &art_txt);
    else { print_colored(BOLD_BLUE_COLOR, "LunaR - The Luna REPL\n"); }

    printf("Version  : ");
    print_colored(BOLD_GREEN_COLOR, "0.0.1 (dev)\n");
    printf("Platform : ");
    print_colored(BOLD_GREEN_COLOR, PLATFORM);
    printf("\n");
    print_colored(YELLOW_COLOR, "\n[!] Type 'info' for configuration info or 'exit' to quit.\n");

    /*******************************************************************************
     * This part creates and initializes the VM used to execute our user's program(s)
     ******************************************************************************/
    VM* vm = initialize_vm();
    if (vm == NULL) {
        print_error("Failed to initialize Virtual Machine.\n");
        exit(1);
    }

    /*******************************************************************************
     * Now we enter the actual REPL (Read-Evaluate-Print-Loop)
     ******************************************************************************/
    for (;;) {
        if (UTF8_SUPPORT == 1) {
            print_colored(BOLD_RED_COLOR, "\n››› ");
        } else {
            print_colored(BOLD_RED_COLOR, "\n>>> ");
        }
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, exit_command) == 0) {
            break;
        } else if (strcmp(input, info_command) == 0) {
            // print info summary
            print_colored(BOLD_MAGENTA_COLOR, "LunaR VM Configuration:\n\n");
            printf("Heap Size            : %u MB\n", (vm->config->heap_size / 1000));
            printf("Heap Entry           : %p\n", vm->heap->entry_ptr);
            printf("Heap End             : %p\n", vm->heap->end_ptr);
            printf("Stack Size           : %u Frames\n", vm->config->stack_size);
            printf("Int64 Registers      : %d\n", vm->config->num_i64_registers);
            printf("Int32 Registers      : %d\n", vm->config->num_i32_registers);
            printf("Int16 Registers      : %d\n", vm->config->num_i16_registers);
            printf("Int8 Registers       : %d\n", vm->config->num_i8_registers);
            printf("Str Registers        : %d\n", vm->config->num_str_registers);
            printf("Emit Instruction Set : %s\n",
                   vm->config->emit_instruction_set == 1 ? "True" : "False");
        } else if (strcmp(input, config_command) == 0) {
            //            if (strcmp(PLATFORM, "windows") == 0) {
            //                system("start vmconf_template");
            //            } else {
            //                system("open vmconf_template");
            //            }
            //            print_colored(YELLOW_COLOR, "Editing VM config...\n");
            break;
        } else {
            printf("[%s]\n\n", input);
            run_repl(vm, input);
        }
    }

    printf("Exiting...\n");
    return shutdown_vm(vm);
}
