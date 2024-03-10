#include "riscv_compiler.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
void handle_ascii_string(char *line, unsigned int *address, FILE *output_file) {
    // extract the string value
    char *string_start = strchr(line, '"');
    if (string_start != NULL) {
        string_start++; 
        char *string_end = strchr(string_start, '"');
        if (string_end != NULL) {
            *string_end = '\0'; 
            // print byte hex value with address
            for (char *ptr = string_start; ptr < string_end; ++ptr) {
                fprintf(output_file, "0x%08X    0x%02X\n", *address, (unsigned char)*ptr);
                (*address)++; 
            }
        }
    }
}


typedef struct {
    char directive[MAX_LINE_LENGTH];
    int size;
} Directive;

void compile_data_segment(FILE *input_file, FILE *output_file) {
    char line[MAX_LINE_LENGTH];
    int in_data_section = 0; // flag if in data
    unsigned int address = 0x10000000; 

    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // check if line has directive
        if (strstr(line, ".data") != NULL) {
            in_data_section = 1;
            continue;
        } else if (strstr(line, ".text") != NULL) {
            
            break;
        }

        
        if (in_data_section) {
            
            char *directive_start = strchr(line, '.');
            if (directive_start != NULL) {
                
                char directive_name[MAX_LINE_LENGTH];
                sscanf(directive_start, ".%s", directive_name);

                
                FILE *directives_file = fopen("directives.txt", "r");
                if (directives_file == NULL) {
                    printf("Error: Unable to open directives file\n");
                    return;
                }

                // find in directives.txt
                char current_directive[MAX_LINE_LENGTH];
                int offset = 0;
                while (fgets(current_directive, MAX_LINE_LENGTH, directives_file) != NULL) {
                    
                    current_directive[strcspn(current_directive, "\n")] = '\0';

                    // get directive and offset
                    char directive[MAX_LINE_LENGTH];
                    if (sscanf(current_directive, "%s %d", directive, &offset) != 2) {
                        printf("Error: Invalid directive format in directives.txt\n");
                        fclose(directives_file);
                        return;
                    }

                    
                    if (strcmp(directive, directive_name) == 0) {
                        break;
                    }
                }

                
                fclose(directives_file);

                // if array
                if (strcmp(directive_name, "word") == 0 || strcmp(directive_name, "dword") == 0 ||
                    strcmp(directive_name, "byte") == 0 || strcmp(directive_name, "half") == 0 ||
                    strcmp(directive_name, "asciz") == 0) {
                    //get array values
                    char *values_start = strchr(line, '.');
                    if (values_start != NULL) {
                        values_start += strlen(directive_name); 
                        char *token = strtok(values_start, ", \t\n"); 

                        
                        while (token != NULL) {
                            
                            unsigned int data_value = 0;
                            if (strcmp(directive_name, "asciz") == 0) {
                                handle_ascii_string(token, &address, output_file);
                            } else {
                                data_value = (unsigned int)strtol(token, NULL, 0);
                                // print in hex
                                fprintf(output_file, "0x%08X    0x%08X\n", address, data_value);
                            }

                            
                            address += offset;

                            
                            token = strtok(NULL, ", \t\n");
                        }
                    }
                }
            }
        }
    }
}
