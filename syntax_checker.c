#include "syntax_checker.h"
#include <string.h>

#define MAX_LINE_LENGTH 256

int check_syntax(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;
    int skip_until_text = 0;
    FILE *instructions_file = fopen("instructions.txt", "r");

    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return 1;
    }

    error_count += check_for_stray_commas(file);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        char *token = strtok(line, " ,\t\n");

        if (token != NULL && token[0] != '#') {
            if (skip_until_text) {
                if (strcmp(token, ".text") == 0) {
                    skip_until_text = 0;
                }
                continue;
            }

            if (strcmp(token, ".data") == 0) {
                skip_until_text = 1;
                continue;
            }

            char instruction_name[MAX_LINE_LENGTH];
            strcpy(instruction_name, token);

            int found = 0;
            char instr[MAX_LINE_LENGTH];
            int expected_arguments;
            while (fscanf(instructions_file, "%s %d", instr, &expected_arguments) != EOF) {
                if (strcmp(instr, instruction_name) == 0) {
                    found = 1;
                    break;
                }
            }

            rewind(instructions_file);

            if (!found) {
                printf("Syntax Error: Unsupported instruction '%s' on line %d\n", instruction_name, line_number);
                error_count++;
            } else {
                int argument_count = 0;
                while (token != NULL) {
                    token = strtok(NULL, " ,\t\n");
                    argument_count++;
                }
                if (argument_count - 1 != expected_arguments) {
                    printf("Syntax Error: Incorrect number of arguments for instruction '%s' on line %d\n", instruction_name, line_number);
                    error_count++;
                }
            }
        }
    }

    fclose(instructions_file);
    return error_count;
}

int check_for_stray_commas(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        int len = strlen(line);
        if (len > 1 && line[len - 2] == ',') {
            printf("Syntax Error: Stray comma found at the end of line %d\n", line_number);
            error_count++;
        }
        if (feof(file))
            break;
    }

    rewind(file);
    return error_count;
}

void skip_until_text_directive(FILE *file) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char *token = strtok(line, " ,\t\n");
        if (token != NULL && strcmp(token, ".text") == 0) {
            break;
        }
    }
}
