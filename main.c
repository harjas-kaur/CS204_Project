#include <stdio.h>
#include "syntax_checker.h"
#include "directive_checker.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    char *extension = strrchr(filename, '.');
    if (extension == NULL || strcmp(extension, ".asm") != 0) {
        printf("Error: Not a .asm file.\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return 1;
    }

    int syntax_result = check_syntax(file);
    fclose(file); // Close the file before reopening

    file = fopen(filename, "r"); // Reopen the file
    if (file == NULL) {
        printf("Error: Unable to reopen file %s\n", filename);
        return 1;
    }

    int directive_result = check_directives(file);
    fclose(file);

    if (syntax_result == 0 && directive_result == 0) {
        printf("No syntax errors found in %s\n", filename);
    }

    return syntax_result + directive_result;
}


