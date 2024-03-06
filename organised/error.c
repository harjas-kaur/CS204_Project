#include "error.h"
#include <string.h>
#include <ctype.h>
#include<stdio.h>
#define MAX_LINE_LENGTH 256

// Function declarations for internal use

static int check_for_stray_commas(FILE *file);
static void skip_until_text_directive(FILE *file);


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

int check_directives(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;

        // Skip empty lines
        if (strlen(line) == 0 || line[0] == '\n')
            continue;

        // Skip lines starting with comments
        if (line[0] == '#')
            continue;

        // Check if the line contains a label followed by a directive
        char *label_end = strchr(line, ':');
        char *directive_start = strchr(line, '.');

        if (label_end != NULL && directive_start != NULL && directive_start > label_end) {
            // Extract the directive name
            char directive_name[MAX_LINE_LENGTH];
            sscanf(directive_start, ".%s", directive_name);

            // Open the directives file for comparison
            FILE *directives_file = fopen("directives.txt", "r");
            if (directives_file == NULL) {
                printf("Error: Unable to open directives file\n");
                return 1;
            }

            // Check if the directive exists in directives.txt
            char current_directive[MAX_LINE_LENGTH];
            int found = 0;
            while (fgets(current_directive, MAX_LINE_LENGTH, directives_file) != NULL) {
                // Remove newline character
                current_directive[strcspn(current_directive, "\n")] = 0;

                if (strcmp(current_directive, directive_name) == 0) {
                    found = 1;
                    break;
                }
            }

            // Close the directives file
            fclose(directives_file);

            if (!found) {
                printf("Syntax Error: Unsupported directive '%s' on line %d\n", directive_name, line_number);
                error_count++;
            }
        } else if (directive_start != NULL) {
            // Extract the directive name
            char directive_name[MAX_LINE_LENGTH];
            sscanf(directive_start, ".%s", directive_name);

            // Open the directives file for comparison
            FILE *directives_file = fopen("directives.txt", "r");
            if (directives_file == NULL) {
                printf("Error: Unable to open directives file\n");
                return 1;
            }

            // Check if the directive exists in directives.txt
            char current_directive[MAX_LINE_LENGTH];
            int found = 0;
            while (fgets(current_directive, MAX_LINE_LENGTH, directives_file) != NULL) {
                // Remove newline character
                current_directive[strcspn(current_directive, "\n")] = 0;

                if (strcmp(current_directive, directive_name) == 0) {
                    found = 1;
                    break;
                }
            }

            // Close the directives file
            fclose(directives_file);

            if (!found) {
                printf("Syntax Error: Unsupported directive '%s' on line %d\n", directive_name, line_number);
                error_count++;
            }
        }
    }

    rewind(file);
    return error_count;
} 
 int check_errors(const char *filename) {
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
        return 0;
    } else{
        return syntax_result + directive_result;
    }

   
}


