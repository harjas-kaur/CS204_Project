#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_compiler.h"
char* int_to_binary(int value, int num_bits) {
    static char binary_string[33]; // 32-bit integer null 
    binary_string[num_bits] = '\0'; 

    for (int i = num_bits - 1; i >= 0; i--) {
        binary_string[i] = (value & 1) ? '1' : '0'; // lsb
        value >>= 1; // shift right by 1 bit
    }

    return binary_string;
}
char *binary_to_hex(const char *binary) {
    // convert binary string to integer
    int decimal = strtol(binary, NULL, 2);
    // convert integer to hexadecimal string
    static char hex[9]; 
    sprintf(hex, "%08X", decimal);
    return hex;
}

char* decimal_to_binary(char* decimal_str) {
    int decimal = atoi(decimal_str);  // string to integer
    char* binary = (char*)malloc(6 * sizeof(char));
    if (binary == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // conversion
    for (int i = 4; i >= 0; i--) {
        binary[i] = (decimal & 1) ? '1' : '0';  
        decimal >>= 1;  
    }
    binary[5] = '\0';  

    return binary;
}


void assemble_r_type(char *line, int *program_counter) {
    // get instruction keyword
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_keyword = strtok(line_copy, " \t\n");
    if (instruction_keyword == NULL) {
        printf("error: Unable to parse instruction keyword\n");
        return;
    }

    //debug
   // printf("instruction line: %s\n", line);

    // extract registers
    char *rd = strtok(NULL, " ");
    char *rs1 = strtok(NULL, " ");
    char *rs2 = strtok(NULL, " ");
    
    //if any register operis missing
    if (rd == NULL || rs1 == NULL || rs2 == NULL) {
        printf("error: Missing register operand\n");
        return;
    }

    // remove 'x' from register operands
    if (rd[0] == 'x') rd++;
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;
        
    FILE *instructions_FILE = fopen("instructions.txt", "r");
   
    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH], funct7[MAX_LINE_LENGTH];
    
    // used instructions FILE to find corresponding opcode, funct3, funct7
    while (fgets(instr_line, sizeof(instr_line), instructions_FILE) != NULL) {
        char instr_keyword[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s %s", instr_keyword, opcode, funct3, funct7) != 4) {
            continue;
        }
        if (strcmp(instr_keyword, instruction_keyword) == 0) {
            printf("Read: %s %s %s %s\n", instr_keyword, opcode, funct3, funct7);
            break;
        }
    }

    fclose(instructions_FILE);
    
    char *rd_binary = decimal_to_binary(rd);
    char *rs1_binary = decimal_to_binary(rs1);
    char *rs2_binary = decimal_to_binary(rs2);

    
    char machine_code[33]; 
    sprintf(machine_code, "%s%s%s%s%s%s", funct7, rs2_binary, rs1_binary, funct3, rd_binary, opcode);
    printf("Machine code: %s\n", machine_code);
    // binary code to hex
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    //program counter
    char pc_hex[9]; 
    sprintf(pc_hex, "0x%02X", *program_counter);

    //output
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s 0x%s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}
void assemble_i_type(char *line, int *program_counter) {
    // extract instruction keyword
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_keyword = strtok(line_copy, " \t\n");
    if (instruction_keyword == NULL) {
        printf("error: Unable to parse instruction keyword\n");
        return;
    }

    // print line to check its content
    printf("Instruction line: %s\n", line);

    // extract rd, rs1, immediate value
    char *rd = strtok(NULL, " ");
    char *rs1 = strtok(NULL, " ");
    char *immediate_value_str = strtok(NULL, " ");

   
    if (rd == NULL || rs1 == NULL || immediate_value_str == NULL) {
        printf("error: Missing operand\n");
        return;
    }

    
    if (rd[0] == 'x') rd++;
    if (rs1[0] == 'x') rs1++;

    // immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    
    FILE *instructions_FILE = fopen("instructions.txt", "r");
    if (instructions_FILE == NULL) {
        printf("error: Unable to open instructions FILE\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    
    while (fgets(instr_line, sizeof(instr_line), instructions_FILE) != NULL) {
        char instr_keyword[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_keyword, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_keyword, instruction_keyword) == 0) {
            printf("Read: %s %s %s\n", instr_keyword, opcode, funct3);
            break;
        }
    }

    fclose(instructions_FILE);

   
    char *rd_binary = decimal_to_binary(rd);
    char *rs1_binary = decimal_to_binary(rs1);
    char *immediate_binary = decimal_to_binary(immediate_value_str);

    
    char machine_code[33];
    sprintf(machine_code, "%s%s%s%s%s", immediate_binary, rs1_binary, funct3, rd_binary, opcode);
    printf("Machine code: %s\n", machine_code);

    
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    
    char pc_hex[9];
    sprintf(pc_hex, "0x%02X", *program_counter);

    // write to output.mc
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s 0x%s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}

void assemble_s_type(char *line, int *program_counter) {
    // extract instruction keyword operands
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_keyword = strtok(line_copy, " \t\n");
    if (instruction_keyword == NULL) {
        printf("error: Unable to parse instruction keyword\n");
        return;
    }

    char *rs2 = strtok(NULL, " ");
    char *offset_str = strtok(NULL, "(");
    char *rs1 = strtok(NULL, ")");
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;
    // check if any oper is missing
    if (rs2 == NULL || offset_str == NULL || rs1 == NULL) {
        printf("error: Missing operand\n");
        return;
    }

    printf("Instruction: %s, rs2: %s, offset: %s, rs1: %s\n", instruction_keyword, rs2, offset_str, rs1);

    // convert immediate -decimal-binary
    int offset_value = atoi(offset_str);
    if (offset_value < -16 || offset_value > 15) {
        printf("error: Immediate value out of range (-16 to 15)\n");
        return;
    }
    char *offset_binary = decimal_to_binary(offset_str);
    printf("Offset binary: %s\n", offset_binary);

    
    char *rs1_binary = decimal_to_binary(rs1);
    char *rs2_binary = decimal_to_binary(rs2);
    printf("rs1 binary: %s, rs2 binary: %s\n", rs1_binary, rs2_binary);

    
    FILE *instructions_FILE = fopen("instructions.txt", "r");
    if (instructions_FILE == NULL) {
        printf("error: Unable to open instructions FILE\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];
    
    
    while (fgets(instr_line, sizeof(instr_line), instructions_FILE) != NULL) {
        char instr_keyword[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_keyword, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_keyword, instruction_keyword) == 0) {
            printf("Read: %s %s %s\n", instr_keyword, opcode, funct3);
            break;
        }
    }

    fclose(instructions_FILE);

    
    char machine_code[33]; 
    sprintf(machine_code, "%s%s%s%s%s", rs2_binary, rs1_binary, funct3, offset_binary, opcode);
    printf("Machine code: %s\n", machine_code);

    
    char *hex_machine_code = binary_to_hex(machine_code);
    printf("Hexadecimal machine code: %s\n", hex_machine_code);

    
    char pc_hex[9];
    sprintf(pc_hex, "0x%02X", *program_counter);

    // write to output.mc
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s 0x%s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}
void assemble_u_type(char *line, int *program_counter) {
    // extract instruction keyword
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_keyword = strtok(line_copy, " \t\n");
    if (instruction_keyword == NULL) {
        printf("error: Unable to parse instruction keyword\n");
        return;
    }

    // print line to check its content
    printf("Instruction line: %s\n", line);

    // extract rd immediate value
    char *rd = strtok(NULL, " ");
    char *immediate_value_str = strtok(NULL, " ");

    // check if any operis missing
    if (rd == NULL || immediate_value_str == NULL) {
        printf("error: Missing operand\n");
        return;
    }

    
    if (rd[0] == 'x') rd++;

    // convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // immediate value- 20 bits
    int immediate_bits = immediate_value & 0xFFFFF;

    // immediate value to binary string
    char immediate_binary[21]; 
    sprintf(immediate_binary, "%020b", immediate_bits);

    
    FILE *instructions_FILE = fopen("instructions.txt", "r");
    if (instructions_FILE == NULL) {
        printf("error: Unable to open instructions FILE\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    
    while (fgets(instr_line, sizeof(instr_line), instructions_FILE) != NULL) {
        char instr_keyword[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_keyword, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_keyword, instruction_keyword) == 0) {
            printf("Read: %s %s %s\n", instr_keyword, opcode, funct3);
            break;
        }
    }

    fclose(instructions_FILE);

    
    char *rd_binary = decimal_to_binary(rd);

    
    char machine_code[33]; 
    sprintf(machine_code, "%s%s%s", immediate_binary, rd_binary, opcode);

    // print binary machine code
    //printf("Binary machine code: %s\n", machine_code);

    
    char hex_machine_code[9]; 
    sprintf(hex_machine_code, "%08X", (unsigned int) strtol(machine_code, NULL, 2)); // leading zeros

    
    char pc_hex[9];
    sprintf(pc_hex, "0x%02X", *program_counter);

    // write to output.mc
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s %s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}


void assemble_uj_type(char *line, int *program_counter) {
    
    const char opcode[] = "1101111"; // opcode 

   
    char *rd_str = strtok(line, " \t\n");  
    char *immediate_value_str = strtok(NULL, " \t\n"); 

    
    if (rd_str == NULL || immediate_value_str == NULL) {
        printf("error: Missing operand\n");
        return;
    }

    
    char *rd = rd_str;
    if (rd[0] == 'x') rd++;

    
    int immediate_value = atoi(immediate_value_str);

    // calculate offset -20 bits
    int offset = immediate_value - (*program_counter + 4); // subtract current pc add 4 for pc+4

    // check range
    if (offset < -(1 << 20) || offset >= (1 << 20)) {
        printf("error: immediate value out of range\n");
        return;
    }

    // extract 20-bit immediate from offset
    int immediate_bits = (offset >> 1) & 0xFFFFF;
    char* immediate_binary = int_to_binary(immediate_bits, 20);
    
    char *rd_binary = decimal_to_binary(rd);
    
    char machine_code[33]; 
    sprintf(machine_code, "%s%s%s", immediate_binary,rd_binary, opcode);
    printf("%s", machine_code);
    
    char hex_machine_code[9]; 
    sprintf(hex_machine_code, "%X", (int) strtol(machine_code, NULL, 2));

    
    char pc_hex[9];
    sprintf(pc_hex, "0x%02X", *program_counter);

    // write to output.mc
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s 0x%s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}

void assemble_sb_type(char *line, int *program_counter) {
    // extract instruction keyword operands
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_keyword = strtok(line_copy, " \t\n");
    char *rs1_str = strtok(NULL, " \t\n");  
    char *rs2_str = strtok(NULL, " \t\n"); 
    char *immediate_value_str = strtok(NULL, " \t\n"); 

   
    if (instruction_keyword == NULL || rs1_str == NULL || rs2_str == NULL || immediate_value_str == NULL) {
        printf("error: Missing operand\n");
        return;
    }

    
    char *rs1 = rs1_str;
    char *rs2 = rs2_str;
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;

    // convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // calculate offset 
    int offset = immediate_value - (*program_counter + 4); // Subtract current pC add 4 for pC+4

    if (offset < -(1 << 11) || offset >= (1 << 11)) {
        printf("error: Immediate value out of range\n");
        return;
    }

    // extract 12-bit immediate value from offset
    int immediate_bits = (offset >> 1) & 0xFFF;

    // extract bits 1-4 11 store in rd
    int rd_bits = ((offset >> 11) & 0x1) | ((offset >> 4) & 0xF);

    printf("Immediate bits: %d\n", immediate_bits);
    printf("RD bits: %d\n", rd_bits);

    
    FILE *instructions_FILE = fopen("instructions.txt", "r");
    if (instructions_FILE == NULL) {
        printf("error: Unable to open instructions FILE\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    
    while (fgets(instr_line, sizeof(instr_line), instructions_FILE) != NULL) {
        char instr_keyword[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_keyword, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_keyword, instruction_keyword) == 0) {
            break;
        }
    }

    fclose(instructions_FILE);

    
    char immediate_binary[13]; 
    sprintf(immediate_binary, "%012d", immediate_bits);

    
    char rd_binary[5]; 
    sprintf(rd_binary, "%04d", rd_bits);

    
    char *rs2_binary = decimal_to_binary(rs2);
    char *rs1_binary = decimal_to_binary(rs1);

    
    char machine_code[33]; 
    sprintf(machine_code, "%s%s%s%s%s", immediate_binary, rs2_binary, rs1_binary, opcode, funct3);

    // rd bits into mc at correct position
    machine_code[7] = rd_binary[0];
    machine_code[8] = rd_binary[1];
    machine_code[9] = rd_binary[2];
    machine_code[10] = rd_binary[3];

    printf("Machine code (binary): %s\n", machine_code);

    
    char hex_machine_code[9]; 
    sprintf(hex_machine_code, "%X", (int) strtol(machine_code, NULL, 2));

    
    char pc_hex[9];
    sprintf(pc_hex, "0x%02X", *program_counter);

    // write to output.mc
    FILE *output_FILE = fopen("output.mc", "a"); 
    if (output_FILE == NULL) {
        printf("error: Unable to open output FILE\n");
        return;
    }
    fprintf(output_FILE, "%s 0x%s\n", pc_hex, hex_machine_code);

    
    *program_counter += 4;

    
    fclose(output_FILE);
}
