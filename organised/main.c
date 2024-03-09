#include <stdio.h>
#include "error.h"
#include<string.h>
#include "riscv_compiler.h"
//test file for error checking
//for testing gcc -o syntax_checker main.c error.c text.c data.c -std=c99 -Wall -Wextra
//can run compiled like ./syntax_checker test.asm
//issues arise when compiling on wsl. reopen the terminal. it will reflect changes.
void assemble_data_segment(){
    // Open the RISC-V file
    FILE *riscv_file = fopen("test.asm", "r");
    if (riscv_file == NULL) {
        printf("Error: Unable to open RISC-V file\n");
        return;
    }

    // Create or open the output file
    FILE *output_file = fopen("output.mc", "w");
    if (output_file == NULL) {
        printf("Error: Unable to create/open output file\n");
        fclose(riscv_file);
        return ;
    }

    // Compile the data segment
    compile_data_segment(riscv_file, output_file);

    // Close the files
    fclose(riscv_file);
    fclose(output_file);

    return ;
}

void assemble_text_segment() {
    int program_counter=0;
    int *program_counter_ptr = &program_counter;
    FILE *input_file = fopen("test.asm", "r");
    if (input_file == NULL) {
        printf("Error: Unable to open input file\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char line_copy[MAX_LINE_LENGTH];
    // Iterate through each line in the input file
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // Skip empty lines and comments
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == '\n') {
            continue;
        }

        printf("Processing line: %s\n", line);
        strcpy(line_copy, line);
        // Tokenize the line to get the instruction mnemonic
        char *instruction_mnemonic = strtok(line, " \t\n");
        if (instruction_mnemonic == NULL) {
            printf("Error: Unable to parse instruction mnemonic\n");
            continue;
        }
        //printf("Instruction mnemonic after tokenization: %s\n", instruction_mnemonic);
        
        // Read instruction details from instructions.txt
        FILE *instructions_file = fopen("instructions.txt", "r");
        if (instructions_file == NULL) {
            printf("Error: Unable to open instructions file\n");
            perror("fopen");
            fclose(input_file);
            return;
        }
        
        char instr_line[MAX_LINE_LENGTH];
        while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
            //printf("Inside the while loop\n");
            //printf("Instruction line from instructions.txt: %s", instr_line); // Print the line from instructions.txt
            char instr_mnemonic[MAX_LINE_LENGTH];
            char instr_type[MAX_LINE_LENGTH];
            if (sscanf(instr_line, "%s %s", instr_type, instr_mnemonic) != 2) {
                continue;
            }
            
            //printf("Instruction read from instructions.txt: %s %s\n", instr_mnemonic, instr_type);
            if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
                // Call the appropriate assembly function based on the instruction type
                if (strcmp(instr_type, "R") == 0) {
                    printf("Assembling R-type instruction\n");
                    assemble_r_type(line_copy, program_counter_ptr);
                    // Call the function to assemble R-type instruction
                } else if (strcmp(instr_type, "I") == 0) {
                    printf("Assembling I-type instruction\n");
                    assemble_i_type(line_copy, program_counter_ptr);
                    // Call the function to assemble I-type instruction
                } else if (strcmp(instr_type, "S") == 0) {
                    printf("Assembling S-type instruction\n");
                    // Call the function to assemble S-type instruction
                    assemble_s_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "SB") == 0) {
                    printf("Assembling SB-type instruction\n");
                    // Call the function to assemble SB-type instruction
                } else if (strcmp(instr_type, "U") == 0) {
                    printf("Assembling U-type instruction\n");
                    // Call the function to assemble U-type instruction
                    assemble_u_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "UJ") == 0) {
                    printf("Assembling UJ-type instruction\n");
                    // Call the function to assemble UJ-type instruction
                } else {
                    printf("Error: Unknown instruction type '%s'\n", instr_type);
                }
            }
        }

        if (feof(instructions_file)) {
            //printf("End of instructions file reached\n");
        } else if (ferror(instructions_file)) {
            printf("Error reading instructions file\n");
        }

        fclose(instructions_file);
    }

    fclose(input_file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // assemble_text_segment();
    

    const char *filename = argv[1];
    int result = check_errors(filename);

    
    //printf("Result: %d\n", result); // Print the result
    if(result!=0){
        printf("Can not assemble! Fix errors!\n");
        return 0;
    } else{
        assemble_data_segment();
        //printf("Debug 1\n");
        assemble_text_segment();
        //printf("Debug 2\n");
        return 0;
    }
    
}
