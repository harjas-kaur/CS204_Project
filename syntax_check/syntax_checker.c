#include "syntax_checker.h"
#include <ctype.h>
#include <string.h>

// Function declarations for internal use
static int check_for_stray_commas(FILE *file);
static void skip_until_text_directive(FILE *file);

#define MAX_LINE_LENGTH 256

static int is_valid_label(const char *label) {
    // Check if the label is a valid RISC-V label
    if (!isalpha(label[0])) {
        return 0; // Labels must start with a letter
    }

    for (size_t i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i]) && label[i] != '_') {
            return 0; // Labels may contain letters, digits, and underscores
        }
    }

    return 1;
}

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

            // Check if the token is a label
            if (token[strlen(token) - 1] == ':') {
                // Remove the colon from the label
                token[strlen(token) - 1] = '\0';
                if (!is_valid_label(token)) {
                    printf("Syntax Error: Invalid label '%s' on line %d\n", token, line_number);
                    error_count++;
                }
                continue; // Ignore labels
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

static int check_for_stray_commas(FILE *file) {
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

static void skip_until_text_directive(FILE *file) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char *token = strtok(line, " ,\t\n");
        if (token != NULL && strcmp(token, ".text") == 0) {
            break;
        }
    }
}
