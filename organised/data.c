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
            // Update address
            *address += strlen(string_start); // Assuming each character in the string is one byte
            // Print the address and string
            fprintf(output_file, "0x%08X    %s\n", *address, string_start);
        }
    }
}
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

                // Check if it's an array declaration
                if (strcmp(directive_name, "word") == 0) {
                    // Extract the values of the array
                    char *values_start = strchr(line, '.');
                    if (values_start != NULL) {
                        values_start += strlen("word"); // Move pointer to the values part
                        char *token = strtok(values_start, ", \t\n"); // Tokenize based on comma, space, or tab

                        // Parse and print each value
                        while (token != NULL) {
                            // Update address
                            address += 4; // Assuming each value in the array is 4 bytes (32 bits)

                            // Print the address and data in hexadecimal format
                            fprintf(output_file, "0x%08X    0x%s\n", address, token); // Assuming data is in hex format
                            token = strtok(NULL, ", \t\n");
                        }
                    }
                } else if (strcmp(directive_name, "asciiz") == 0) {
                    // Jump to function to handle ASCII string
                    // You can define another function to handle ASCII strings
                    // and call it here passing necessary arguments
                    // For simplicity, let's assume a function called handle_ascii_string
                    handle_ascii_string(line, &address, output_file);
                }
            }
        }
    }
}


