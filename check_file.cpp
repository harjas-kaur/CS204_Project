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

    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        char *token = strtok(line, " ,\t\n");
        if (token != NULL && token[0] != '#') { // Ignore comments and empty lines
            char instruction_name[MAX_LINE_LENGTH];
            strcpy(instruction_name, token);

            // Find instruction in the instructions file
            int found = 0;
            char instr[MAX_LINE_LENGTH];
            int expected_arguments;
            while (fscanf(instructions_file, "%s %d", instr, &expected_arguments) != EOF) {
                if (strcmp(instr, instruction_name) == 0) {
                    found = 1;
                    break;
                }
            }

            // Reset file pointer to beginning for next iteration
            rewind(instructions_file);

            if (!found) {
                printf("Syntax Error: Unsupported instruction '%s' on line %d\n", instruction_name, line_number);
                error_count++;
            } else {
                // Check number of arguments
                int argument_count = 0;
                while (token != NULL) {
                    token = strtok(NULL, " ,\t\n");
                    argument_count++;
                }
                if (argument_count - 1 != expected_arguments) { // Subtract 1 for the instruction name
                    printf("Syntax Error: Incorrect number of arguments for instruction '%s' on line %d\n", instruction_name, line_number);
                    error_count++;
                }
            }
        }
    }

    fclose(instructions_file);
    return error_count;
}

