// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "io.h"
#include "repl.h"
#include <stdlib.h>

char* read_file(const char* path) {
    FILE* fp;
    long file_size;
    char* file_content;

    fp = fopen(path, "r");
    if (fp == NULL) {
        print_error("Error opening file\n");
        exit(1);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    // Allocate memory for file content
    file_content = (char*) malloc(file_size + 1);
    if (file_content == NULL) {
        fclose(fp);
        print_error("Error allocating memory for file content\n");
        exit(1);
    }

    // Read the file content
    fread(file_content, sizeof(char), file_size, fp);
    file_content[file_size] = '\0';

    // Close the file
    fclose(fp);

    // Remove newlines
    char* p = file_content;
    while (*p != '\0') {
        if (*p != '\n') { *file_content++ = *p; }
        p++;
    }
    *file_content = '\0';

    return file_content - file_size;
}
