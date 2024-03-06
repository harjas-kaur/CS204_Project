#include "riscv_compiler.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
void handle_ascii_string(char *line, unsigned int *address, FILE *output_file) {
    // Extract the string value
    char *string_start = strchr(line, '"');
    if (string_start != NULL) {
        string_start++; // Move pointer to the start of the string
        char *string_end = strchr(string_start, '"');
        if (string_end != NULL) {
            *string_end = '\0'; // Null-terminate the string
            // Print each byte's hexadecimal value along with its address
            for (char *ptr = string_start; ptr < string_end; ++ptr) {
                fprintf(output_file, "0x%08X    0x%02X\n", *address, (unsigned char)*ptr);
                (*address)++; // Move to the next address
            }
        }
    }
}

// Structure to hold directive type and its size
typedef struct {
    char directive[MAX_LINE_LENGTH];
    int size;
} Directive;

void compile_data_segment(FILE *input_file, FILE *output_file) {
    char line[MAX_LINE_LENGTH];
    int in_data_section = 0; // Flag to indicate if currently in the data section
    unsigned int address = 0x10000000; // Starting address

    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // Check if the line contains a directive
        if (strstr(line, ".data") != NULL) {
            in_data_section = 1;
            continue;
        } else if (strstr(line, ".text") != NULL) {
            // Exit the loop if reached the text section
            break;
        }

        // If in the data section
        if (in_data_section) {
            // Check if the line contains a directive
            char *directive_start = strchr(line, '.');
            if (directive_start != NULL) {
                // Extract the directive name
                char directive_name[MAX_LINE_LENGTH];
                sscanf(directive_start, ".%s", directive_name);

                // Open the directives file for comparison
                FILE *directives_file = fopen("directives.txt", "r");
                if (directives_file == NULL) {
                    printf("Error: Unable to open directives file\n");
                    return;
                }

                // Find the directive in directives.txt
                char current_directive[MAX_LINE_LENGTH];
                int offset = 0;
                while (fgets(current_directive, MAX_LINE_LENGTH, directives_file) != NULL) {
                    // Remove newline character
                    current_directive[strcspn(current_directive, "\n")] = '\0';

                    // Extract the directive and offset
                    char directive[MAX_LINE_LENGTH];
                    if (sscanf(current_directive, "%s %d", directive, &offset) != 2) {
                        printf("Error: Invalid directive format in directives.txt\n");
                        fclose(directives_file);
                        return;
                    }

                    // Check if the directive matches
                    if (strcmp(directive, directive_name) == 0) {
                        break;
                    }
                }

                // Close the directives file
                fclose(directives_file);

                // Check if it's an array declaration
                if (strcmp(directive_name, "word") == 0 || strcmp(directive_name, "dword") == 0) {
                    // Extract the values of the array
                    char *values_start = strchr(line, '.');
                    if (values_start != NULL) {
                        values_start += strlen(directive_name); // Move pointer to the values part
                        char *token = strtok(values_start, ", \t\n"); // Tokenize based on comma, space, or tab

                        // Parse and print each value
                        while (token != NULL) {
                            // Convert token to integer
                            unsigned int data_value = (unsigned int)strtol(token, NULL, 0);

                            // Print the address and data in hexadecimal format
                            fprintf(output_file, "0x%08X    0x%08X\n", address, data_value);

                            // Update address based on the data type
                            address += offset;

                            // Get the next token
                            token = strtok(NULL, ", \t\n");
                        }
                    }
                }
            }
        }
    }
}