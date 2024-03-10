#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "riscv_compiler.h"
#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100
//use makefile to run labels.c first and then rest of it with the modified test.asm
// gcc labels.c errors.c
//ascii string is not compiling
typedef struct {
    char label[50];
    int address;
} LabelEntry;

LabelEntry labelTable[MAX_LABELS];
int labelCount = 0;

void handle_labels(char *input_file_name) {
    FILE *input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("Error: Unable to open input file\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int program_counter = 0;  // Initialize program counter

    // Iterate through each line in the input file
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // Check if the line is a label
        char *colon_pos = strchr(line, ':');
        if (colon_pos != NULL) {
            // Extract label and add it to the label table
            *colon_pos = '\0';  // Null terminate to remove label part
            char *label = strtok(line, " \t\n");
            strcpy(labelTable[labelCount].label, label);
            // Set the address of the label to the current program counter
            labelTable[labelCount].address = program_counter;
            labelCount++;

            // Skip the label part in the input file
            char *instruction_start = colon_pos + 1;
            while (*instruction_start == ' ' || *instruction_start == '\t') {
                instruction_start++;
            }

            // Print the instruction for debugging
            //printf("Instruction: %s\n", instruction_start);

            // Increment the program counter for the instruction part
            program_counter += 4;

            continue;
        }

        // If it's not a label, check if there's any text on the line
        char *token = strtok(line, " \t\n");
        if (token != NULL) {
            // Increment the program counter for each non-empty instruction line
            program_counter += 4;
        }
    }

    fclose(input_file);
}
void remove_labels(char *input_file_name) {
    FILE *input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("Error: Unable to open input file\n");
        return;
    }

    char line[MAX_LINE_LENGTH];

    FILE *temp_file = fopen("temp.asm", "w");
    if (temp_file == NULL) {
        printf("Error: Unable to create temporary file\n");
        fclose(input_file);
        return;
    }

    // Iterate through each line in the input file
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // Check if the line contains a label
        char *colon_pos = strchr(line, ':');
        if (colon_pos != NULL) {
            // Skip writing the label part to the temporary file
            char *instruction_start = colon_pos + 1;
            // Trim leading spaces
            while (*instruction_start == ' ' || *instruction_start == '\t') {
                instruction_start++;
            }
            fputs(instruction_start, temp_file);
            continue;
        }

        // Write non-label lines to the temporary file
        fputs(line, temp_file);
    }

    fclose(input_file);
    fclose(temp_file);

    // Remove original input file and rename temporary file
    remove(input_file_name);
    rename("temp.asm", input_file_name);
}
void replace_labels(char *input_file_name) {
    FILE *input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("Error: Unable to open input file\n");
        return;
    }
    

    char line[MAX_LINE_LENGTH];
    FILE *temp_file = fopen("temp.asm", "w");
    if (temp_file == NULL) {
        printf("Error: Unable to create temporary file\n");
        fclose(input_file);
        return;
    }
    

    // Iterate through each line in the input file
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        
        // Tokenize the line to get the instruction and its arguments
        char *instruction = strtok(line, " \t\n");
        if (instruction == NULL) {
            //printf("Error: Failed to tokenize instruction\n");
            continue;
        }
        fprintf(temp_file, "%s", instruction); // Write the instruction to the temporary file
        
        // Process the arguments of the instruction
        char *arguments = strtok(NULL, "\n"); // Get the rest of the line as arguments
        if (arguments != NULL) {
            // Tokenize the arguments until the last one
            char *arg = strtok(arguments, ",");
            while (arg != NULL) {
                // Check if the argument matches any label in the label table
                int label_found = 0;
                for (int i = 0; i < labelCount; i++) {
                    if (strcmp(arg, labelTable[i].label) == 0) {
                        // Replace label with its corresponding address only for SB and UJ instructions
                        if (strcmp(instruction, "beq") == 0 || strcmp(instruction, "bne") == 0 ||
                            strcmp(instruction, "bge") == 0 || strcmp(instruction, "blt") == 0 ||
                            strcmp(instruction, "jal") == 0) {
                            fprintf(temp_file, " %d", labelTable[i].address);
                            label_found = 1;
                            
                        } else {
                            fprintf(temp_file, " %s", arg); // Keep the label as is for other instructions
                            
                        }
                        break;
                    }
                }
                // If the argument is not a label, write it as is
                if (!label_found) {
                    fprintf(temp_file, " %s", arg);
                    
                }
                arg = strtok(NULL, ",");
            }
        }
        fputs("\n", temp_file); // Write newline character after each line
        
    }

    fclose(input_file);
    fclose(temp_file);

    // Remove original input file and rename temporary file
    remove(input_file_name);
    rename("temp.asm", input_file_name);
}

void delete_data_segment(const char *filename) {
    // Open the file for reading and writing
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char line[256];
    int text_start = 0;
    int line_number = 0;

    // Find the start position of the .text segment
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        // Check if the line contains ".text"
        if (strstr(line, ".text") != NULL) {
            text_start = line_number;
            break;
        }
    }

    // If .text segment is found, move the content after text_start to the beginning of the file
    if (text_start > 0) {
        fseek(file, 0, SEEK_SET); // Move file pointer to the start of the file

        // Create a temporary file to store lines without the data segment
        FILE *temp_file = fopen("temp.asm", "w");
        if (temp_file == NULL) {
            printf("Error: Unable to create temporary file\n");
            fclose(file);
            return;
        }

        // Copy lines from original file to temporary file starting from text_start
        line_number = 0;
        while (fgets(line, sizeof(line), file) != NULL) {
            line_number++;
            if (line_number > text_start) {
                fputs(line, temp_file);
            }
        }

        // Close both files
        fclose(file);
        fclose(temp_file);

        // Remove the original file
        if (remove(filename) != 0) {
            printf("Error: Unable to delete original file\n");
            return;
        }

        // Rename the temporary file to the original filename
        if (rename("temp.asm", filename) != 0) {
            printf("Error: Unable to rename temporary file\n");
            return;
        }

        printf("Data segment deleted successfully\n");
    } else {
        printf("Error: .text segment not found\n");
    }
}

int main() {
    const char *filename = "test.asm";

    // Check for errors in the assembly file
    int result = check_errors(filename);
    if (result != 0) {
        printf("Can not assemble! Fix errors!\n");
        return 0;
    }

    // Open input and output files
    FILE *input_file = fopen(filename, "r");
    FILE *output_file = fopen("output.mc", "w");
    if (input_file == NULL || output_file == NULL) {
        printf("Error: Unable to open files\n");
        return 1;
    }

    // Compile the data segment
    compile_data_segment(input_file, output_file);
    delete_data_segment(filename);

    // Close the files
    fclose(input_file);
    fprintf(output_file, "<end of data segment...>\n");
    fclose(output_file);

    // Handle labels
    handle_labels(filename);

    // Print the label table for debugging
    printf("Label Table:\n");
    for (int i = 0; i < labelCount; i++) {
        printf("Label: %s, Address: %d\n", labelTable[i].label, labelTable[i].address);
    }

    // Remove and replace labels
    remove_labels(filename);
    replace_labels(filename);

    return 0;
}