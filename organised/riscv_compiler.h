#ifndef RISCV_COMPILER_H
#define RISCV_COMPILER_H
#define MAX_LINE_LENGTH 100
#include <stdio.h>

void compile_data_segment(FILE *input_file, FILE *output_file);

// Function to check if a string represents a register (starts with 'x')
int is_register(const char *operand);

// Function to extract the numerical value from a register string
int extract_register_value(const char *operand);

// Function to convert binary string to hexadecimal string
char* binary_to_hexadecimal(const char *binary);

void assemble_r_type(const char *instruction);
void assemble_i_type(const char *instruction);
void assemble_s_type(const char *instruction);
void assemble_sb_type(const char *instruction);
void assemble_u_type(const char *instruction);
void assemble_uj_type(const char *instruction);

#endif /* RISCV_COMPILER_H */
