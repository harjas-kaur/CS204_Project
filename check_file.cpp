#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

// Function prototypes
int check_syntax(FILE *file);

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
    fclose(file);

    if (syntax_result == 0) {
        printf("No syntax errors found in %s\n", filename);
    }

    return syntax_result;
}

int check_syntax(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;

    // Supported instructions
    const char *supported_instructions[] = {
        "add", "and", "or", "sll", "slt", "sra", "srl", "sub", "xor", "mul", "div", "rem",
        "addi", "andi", "ori", "lb", "ld", "lh", "lw", "jalr",
        "sb", "sw", "sd", "sh",
        "beq", "bne", "bge", "blt",
        "auipc", "lui",
        "jal"
    };

    const int num_instructions = sizeof(supported_instructions) / sizeof(supported_instructions[0]);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        char *token = strtok(line, " ,\t\n");
        if (token != NULL && token[0] != '#') { // Ignore comments and empty lines
            int valid_instruction = 0;
            for (int i = 0; i < num_instructions; i++) {
                if (strcmp(token, supported_instructions[i]) == 0) {
                    valid_instruction = 1;
                    break;
                }
            }
            if (!valid_instruction) {
                printf("Syntax Error: Unsupported instruction on line %d\n", line_number);
                error_count++;
            }
        }
    }

    return error_count;
}
