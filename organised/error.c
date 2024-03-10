#include "error.h"
#include <string.h>
#include <ctype.h>
#include<stdio.h>
#define MAX_LINE_LENGTH 256

static int check_for_stray_commas(FILE *file);

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

            // check label
            if (token[strlen(token) - 1] == ':') {
                token[strlen(token) - 1] = '\0';
                
                continue; // ignore label
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


int check_directives(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;

        // empty lines 
        if (strlen(line) == 0 || line[0] == '\n' || line[0] == '#')
            continue;

        // check if label and directive 
        char *label_end = strchr(line, ':');
        char *directive_start = strchr(line, '.');

        // Skip lines containing an integer separated by a tab
        char *tab_position = strchr(line, '\t');
        if (tab_position != NULL && tab_position > directive_start) {
            continue;
        }

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

            // check if the directive exists in directives.txt
            char current_directive[MAX_LINE_LENGTH];
            int found = 0;
            while (fgets(current_directive, MAX_LINE_LENGTH, directives_file) != NULL) {
                // Remove newline character
                current_directive[strcspn(current_directive, "\n")] = 0;

                // Extract directive name from the line in directives.txt
                char current_directive_name[MAX_LINE_LENGTH];
                sscanf(current_directive, "%s", current_directive_name);

                if (strcmp(current_directive_name, directive_name) == 0) {
                    found = 1;
                    break;
                }
            }

            // close the directives file
            fclose(directives_file);

            if (!found) {
                printf("Syntax Error: Unsupported directive '%s' on line %d\n", directive_name, line_number);
                error_count++;
            }
        }
    }
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


