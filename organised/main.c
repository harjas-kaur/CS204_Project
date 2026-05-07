#include <stdio.h>
#include<string.h>
#include "riscv_compiler.h"
//test file for error checking
//for testing gcc -o syntax_checker main.c error.c text.c data.c -std=c99 -Wall -Wextra
//can run compiled like ./syntax_checker test.asm
//issues arise when compiling on wsl. reopen the terminal. it will reflect changes.
//beq and jal do not give out correct machine code compared to venus.
void skip_labels(char *line_copy) {
    
    char *colon_pos = strchr(line_copy, ':');

    // colon found then skip label 
    if (colon_pos != NULL) {
        
        size_t label_length = colon_pos - line_copy + 1; // +1 to include the colon
        memmove(line_copy, colon_pos + 1, strlen(colon_pos));
        line_copy[strlen(line_copy) - label_length] = '\0';
    }
}

void assemble_text_segment() {
    int program_counter=0;
    int *program_counter_ptr = &program_counter;
    FILE *input_file = fopen("test.asm", "r");
    if (input_file == NULL) {
        printf("Error: unable to open input file\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char line_copy[MAX_LINE_LENGTH];
    // iterate through each line in the input file
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
            if (strlen(line) <= 1 || line[0] == '#' || line[0] == '\n') {
            continue;
        }

        //printf("line: %s\n", line);
        strcpy(line_copy, line);
        skip_labels(line_copy);
        //printf("line: %s\n", line_copy);

        
        char *instruction_mnemonic = strtok(line, " \t\n");
        if (instruction_mnemonic == NULL) {
            printf("Error: can not get instruction keyword\n");
            continue;
        }
        //printf("Instruction mnemonic after tokenization: %s\n", instruction_mnemonic);
        
        FILE *instructions_file = fopen("instructions.txt", "r");
        
        
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
              
                if (strcmp(instr_type, "R") == 0) {
                    //printf("Assembling R-type instruction\n");
                    assemble_r_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "I") == 0) {
                    //printf("Assembling I-type instruction\n");
                    assemble_i_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "S") == 0) {
                    //printf("Assembling S-type instruction\n");
                    assemble_s_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "SB") == 0) {
                    //printf("Assembling SB-type instruction\n");
                     assemble_sb_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "U") == 0) {
                    //printf("Assembling U-type instruction\n");
                    assemble_u_type(line_copy, program_counter_ptr);
                } else if (strcmp(instr_type, "UJ") == 0) {
                    //printf("Assembling UJ-type instruction\n");
                    assemble_uj_type(line_copy, program_counter_ptr);
                } else {
                    printf("Error: Unknown instruction type '%s'\n", instr_type);
                }
            }
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
    
   // assemble_data_segment();
        //printf("Debug 1\n");
        assemble_text_segment();
        //printf("Debug 2\n");
        return 0;
}
