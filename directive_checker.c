#include "directive_checker.h"
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

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