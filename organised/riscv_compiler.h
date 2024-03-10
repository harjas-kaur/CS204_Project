#ifndef RISCV_COMPILER_H
#define RISCV_COMPILER_H
#define MAX_LINE_LENGTH 100
#include <stdio.h>

void compile_data_segment(FILE *input_file, FILE *output_file);


int is_register(const char *operand);


int extract_register_value(const char *operand);
void assemble_u_type(char *line, int *program_counter);
void assemble_uj_type(char *line, int *program_counter);
void assemble_r_type(char *line, int *program_counter);
void assemble_i_type(char *line, int *program_counter);
void assemble_s_type(char *line, int *program_counter);
void assemble_sb_type(char *line, int *program_counter);
void handle_labels(char *input_file_name);
void remove_labels(char *input_file_name);
void replace_labels(char *input_file_name);
#endif 
