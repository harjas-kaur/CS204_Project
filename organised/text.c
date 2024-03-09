#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_compiler.h"

char *binary_to_hex(const char *binary) {
    // Convert binary string to integer
    int decimal = strtol(binary, NULL, 2);

    // Convert integer to hexadecimal string
    static char hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(hex, "%08X", decimal);

    return hex;
}
char* decimal_to_binary(char* decimal_str) {
    int decimal = atoi(decimal_str);  // Convert string to integer
    char* binary = (char*)malloc(6 * sizeof(char));  // Allocate memory for binary string
    if (binary == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Convert decimal to binary
    for (int i = 4; i >= 0; i--) {
        binary[i] = (decimal & 1) ? '1' : '0';  // Extract the least significant bit
        decimal >>= 1;  // Right shift by 1 bit
    }
    binary[5] = '\0';  // Null-terminate the string

    return binary;
}


void assemble_r_type(char *line, int *program_counter) {
    // Extract instruction mnemonic
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_mnemonic = strtok(line_copy, " \t\n");
    if (instruction_mnemonic == NULL) {
        printf("Error: Unable to parse instruction mnemonic\n");
        return;
    }

    // Print the line to check its content
    printf("Instruction line: %s\n", line);

    // Extract rd, rs1, and rs2
    char *rd = strtok(NULL, ",");
    char *rs1 = strtok(NULL, ",");
    char *rs2 = strtok(NULL, ",");
    
    // Check if any register operand is missing
    if (rd == NULL || rs1 == NULL || rs2 == NULL) {
        printf("Error: Missing register operand\n");
        return;
    }

    // Remove 'x' from register operands
    if (rd[0] == 'x') rd++;
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;
    

    // Open instructions file
    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH], funct7[MAX_LINE_LENGTH];
    
    // Iterate through instructions file to find the corresponding opcode, funct3, and funct7
    while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
        char instr_mnemonic[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s %s", instr_mnemonic, opcode, funct3, funct7) != 4) {
            continue;
        }
        if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
            printf("Read: %s %s %s %s\n", instr_mnemonic, opcode, funct3, funct7);
            break;
        }
    }

    fclose(instructions_file);
    
    char *rd_binary = decimal_to_binary(rd);
    char *rs1_binary = decimal_to_binary(rs1);
    char *rs2_binary = decimal_to_binary(rs2);

    // Generate machine code
    char machine_code[33]; // Increased size to accommodate concatenated strings
    sprintf(machine_code, "%s%s%s%s%s%s", funct7, rs2_binary, rs1_binary, funct3, rd_binary, opcode);
    printf("Machine code: %s\n", machine_code);
    // Convert binary machine code to hexadecimal
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output2.mc
    FILE *output_file = fopen("output2.mc", "a"); // Open output file in append mode
    if (output_file == NULL) {
        printf("Error: Unable to open output file\n");
        return;
    }
    fprintf(output_file, "%s 0x%s\n", pc_hex, hex_machine_code);

    // Increment the program counter
    *program_counter += 4;

    // Close the output file
    fclose(output_file);
}
void assemble_i_type(char *line, int *program_counter) {
    // Extract instruction mnemonic
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_mnemonic = strtok(line_copy, " \t\n");
    if (instruction_mnemonic == NULL) {
        printf("Error: Unable to parse instruction mnemonic\n");
        return;
    }

    // Print the line to check its content
    printf("Instruction line: %s\n", line);

    // Extract rd, rs1, and immediate value
    char *rd = strtok(NULL, ",");
    char *rs1 = strtok(NULL, ",");
    char *immediate_value_str = strtok(NULL, ",");

    // Check if any operand is missing
    if (rd == NULL || rs1 == NULL || immediate_value_str == NULL) {
        printf("Error: Missing operand\n");
        return;
    }

    // Remove 'x' from register operands
    if (rd[0] == 'x') rd++;
    if (rs1[0] == 'x') rs1++;

    // Convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // Open instructions file
    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    // Iterate through instructions file to find the corresponding opcode and funct3
    while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
        char instr_mnemonic[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_mnemonic, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
            printf("Read: %s %s %s\n", instr_mnemonic, opcode, funct3);
            break;
        }
    }

    fclose(instructions_file);

    // Convert operands to binary strings
    char *rd_binary = decimal_to_binary(rd);
    char *rs1_binary = decimal_to_binary(rs1);
    char *immediate_binary = decimal_to_binary(immediate_value_str);

    // Generate machine code
    char machine_code[33]; // Increased size to accommodate concatenated strings
    sprintf(machine_code, "%s%s%s%s%s", immediate_binary, rs1_binary, funct3, rd_binary, opcode);
    printf("Machine code: %s\n", machine_code);

    // Convert binary machine code to hexadecimal
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output2.mc
    FILE *output_file = fopen("output2.mc", "a"); // Open output file in append mode
    if (output_file == NULL) {
        printf("Error: Unable to open output file\n");
        return;
    }
    fprintf(output_file, "%s 0x%s\n", pc_hex, hex_machine_code);

    // Increment the program counter
    *program_counter += 4;

    // Close the output file
    fclose(output_file);
}
// Assemble S-type RISC-V instruction
void assemble_s_type(char *line, int *program_counter) {
    // Extract instruction mnemonic and operands
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_mnemonic = strtok(line_copy, " \t\n");
    if (instruction_mnemonic == NULL) {
        printf("Error: Unable to parse instruction mnemonic\n");
        return;
    }

    char *rs2 = strtok(NULL, ",");
    char *offset_str = strtok(NULL, "(");
    char *rs1 = strtok(NULL, ")");
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;
    // Check if any operand is missing
    if (rs2 == NULL || offset_str == NULL || rs1 == NULL) {
        printf("Error: Missing operand\n");
        return;
    }

    printf("Instruction: %s, rs2: %s, offset: %s, rs1: %s\n", instruction_mnemonic, rs2, offset_str, rs1);

    // Convert immediate to decimal and then to binary
    int offset_value = atoi(offset_str);
    if (offset_value < -16 || offset_value > 15) {
        printf("Error: Immediate value out of range (-16 to 15)\n");
        return;
    }
    char *offset_binary = decimal_to_binary(offset_str);
    printf("Offset binary: %s\n", offset_binary);

    // Convert rs1 and rs2 to binary
    char *rs1_binary = decimal_to_binary(rs1);
    char *rs2_binary = decimal_to_binary(rs2);
    printf("rs1 binary: %s, rs2 binary: %s\n", rs1_binary, rs2_binary);

    // Open instructions file
    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];
    
    // Iterate through instructions file to find the corresponding opcode and funct3
    while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
        char instr_mnemonic[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_mnemonic, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
            printf("Read: %s %s %s\n", instr_mnemonic, opcode, funct3);
            break;
        }
    }

    fclose(instructions_file);

    // Generate machine code
    char machine_code[33]; // Increased size to accommodate concatenated strings
    sprintf(machine_code, "%s%s%s%s%s", rs2_binary, rs1_binary, funct3, offset_binary, opcode);
    printf("Machine code: %s\n", machine_code);

    // Convert binary machine code to hexadecimal
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output2.mc
    FILE *output_file = fopen("output2.mc", "a"); // Open output file in append mode
    if (output_file == NULL) {
        printf("Error: Unable to open output file\n");
        return;
    }
    fprintf(output_file, "%s 0x%s\n", pc_hex, hex_machine_code);

    // Increment the program counter
    *program_counter += 4;

    // Close the output file
    fclose(output_file);
}
void assemble_u_type(char *line, int *program_counter) {
    // Extract instruction mnemonic
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_mnemonic = strtok(line_copy, " \t\n");
    if (instruction_mnemonic == NULL) {
        printf("Error: Unable to parse instruction mnemonic\n");
        return;
    }

    // Print the line to check its content
    printf("Instruction line: %s\n", line);

    // Extract rd and immediate value
    char *rd = strtok(NULL, ",");
    char *immediate_value_str = strtok(NULL, " ");

    // Check if any operand is missing
    if (rd == NULL || immediate_value_str == NULL) {
        printf("Error: Missing operand\n");
        return;
    }

    // Remove 'x' from register operand
    if (rd[0] == 'x') rd++;

    // Convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // Open instructions file
    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    // Iterate through instructions file to find the corresponding opcode and funct3
    while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
        char instr_mnemonic[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_mnemonic, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
            printf("Read: %s %s %s\n", instr_mnemonic, opcode, funct3);
            break;
        }
    }

    fclose(instructions_file);

    // Convert immediate value to binary
    char immediate_binary[21]; // 20 bits for immediate, plus null terminator
    sprintf(immediate_binary, "%020d", immediate_value);

    printf("Immediate binary: %s\n", immediate_binary);

    // Convert rd to binary
    char *rd_binary = decimal_to_binary(rd);

    printf("rd binary: %s\n", rd_binary);

    // Generate machine code
    char machine_code[33]; // Increased size to accommodate concatenated strings
    sprintf(machine_code, "%s%s%s%s%s", immediate_binary, funct3, rd_binary, opcode);
    printf("Machine code: %s\n", machine_code);

    // Convert binary machine code to hexadecimal
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output2.mc
    FILE *output_file = fopen("output2.mc", "a"); // Open output file in append mode
    if (output_file == NULL) {
        printf("Error: Unable to open output file\n");
        return;
    }
    fprintf(output_file, "%s 0x%s\n", pc_hex, hex_machine_code);

    // Increment the program counter
    *program_counter += 4;

    // Close the output file
    fclose(output_file);
}
