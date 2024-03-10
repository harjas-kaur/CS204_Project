#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_compiler.h"
char* int_to_binary(int value, int num_bits) {
    static char binary_string[33]; // Assuming a 32-bit integer + null terminator
    binary_string[num_bits] = '\0'; // Null terminator at the end

    for (int i = num_bits - 1; i >= 0; i--) {
        binary_string[i] = (value & 1) ? '1' : '0'; // Set the least significant bit of the value
        value >>= 1; // Shift the value to the right by 1 bit
    }

    return binary_string;
}
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
    char *rd = strtok(NULL, " ");
    char *rs1 = strtok(NULL, " ");
    char *rs2 = strtok(NULL, " ");
    
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

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
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
    char *rd = strtok(NULL, " ");
    char *rs1 = strtok(NULL, " ");
    char *immediate_value_str = strtok(NULL, " ");

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

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
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

    char *rs2 = strtok(NULL, " ");
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

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
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
    char *rd = strtok(NULL, " ");
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

    // Calculate the immediate value to fit within 20 bits
    int immediate_bits = immediate_value & 0xFFFFF;

    // Convert immediate value to binary string
    char immediate_binary[21]; // 20 bits + null terminator
    sprintf(immediate_binary, "%020b", immediate_bits);

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

    // Convert rd to binary string
    char *rd_binary = decimal_to_binary(rd);

    // Generate machine code
    char machine_code[33]; // 32 bits + null terminator
    sprintf(machine_code, "%s%s%s", immediate_binary, rd_binary, opcode);

    // Debug: Print binary machine code
    printf("Binary machine code: %s\n", machine_code);

    // Convert binary machine code to hexadecimal
    char hex_machine_code[9]; // 8 hex digits + null terminator
    sprintf(hex_machine_code, "%08X", (unsigned int) strtol(machine_code, NULL, 2)); // Pad with leading zeros

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
    if (output_file == NULL) {
        printf("Error: Unable to open output file\n");
        return;
    }
    fprintf(output_file, "%s %s\n", pc_hex, hex_machine_code);

    // Increment the program counter
    *program_counter += 4;

    // Close the output file
    fclose(output_file);
}


void assemble_uj_type(char *line, int *program_counter) {
    // Define the opcode for jal
    const char opcode[] = "1101111"; // Opcode for jal

    // Extract rd and immediate value
    char *rd_str = strtok(line, " \t\n");  // Extracting rd
    char *immediate_value_str = strtok(NULL, " \t\n"); // Extracting immediate value

    // Check if any operand is missing
    if (rd_str == NULL || immediate_value_str == NULL) {
        printf("Error: Missing operand\n");
        return;
    }

    // Remove 'x' from register operand
    char *rd = rd_str;
    if (rd[0] == 'x') rd++;

    // Convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // Calculate the offset (20 bits) for the UJ-type instruction
    int offset = immediate_value - (*program_counter + 4); // Subtract current PC and add 4 for PC+4

    // Check if the immediate value is within the range
    if (offset < -(1 << 20) || offset >= (1 << 20)) {
        printf("Error: Immediate value out of range\n");
        return;
    }

    // Extract the 20-bit immediate value from the offset
    int immediate_bits = (offset >> 1) & 0xFFFFF;
    // Convert immediate value to binary string
    char* immediate_binary = int_to_binary(immediate_bits, 20);
    
    char *rd_binary = decimal_to_binary(rd);
    // Generate machine code
    char machine_code[33]; // 32 bits + null terminator
    sprintf(machine_code, "%s%s%s", immediate_binary,rd_binary, opcode);
    printf("%s", machine_code);
    // Convert binary machine code to hexadecimal
    char hex_machine_code[9]; // 8 hex digits + null terminator
    sprintf(hex_machine_code, "%X", (int) strtol(machine_code, NULL, 2));

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
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

void assemble_sb_type(char *line, int *program_counter) {
    // Extract instruction mnemonic and operands
    char line_copy[256];
    strcpy(line_copy, line);
    char *instruction_mnemonic = strtok(line_copy, " \t\n");
    char *rs1_str = strtok(NULL, " \t\n");  // Extracting rs1
    char *rs2_str = strtok(NULL, " \t\n"); // Extracting rs2
    char *immediate_value_str = strtok(NULL, " \t\n"); // Extracting immediate value

    // Check if any operand is missing
    if (instruction_mnemonic == NULL || rs1_str == NULL || rs2_str == NULL || immediate_value_str == NULL) {
        printf("Error: Missing operand\n");
        return;
    }

    // Remove 'x' from register operands
    char *rs1 = rs1_str;
    char *rs2 = rs2_str;
    if (rs1[0] == 'x') rs1++;
    if (rs2[0] == 'x') rs2++;

    // Convert immediate value string to integer
    int immediate_value = atoi(immediate_value_str);

    // Calculate the offset (12 bits) for the SB-type instruction
    int offset = immediate_value - (*program_counter + 4); // Subtract current PC and add 4 for PC+4

    // Check if the immediate value is within the range
    if (offset < -(1 << 11) || offset >= (1 << 11)) {
        printf("Error: Immediate value out of range\n");
        return;
    }

    // Extract the 12-bit immediate value from the offset
    int immediate_bits = (offset >> 1) & 0xFFF;

    // Extract bits 1-4 and 11 to store in rd
    int rd_bits = ((offset >> 11) & 0x1) | ((offset >> 4) & 0xF);

    printf("Immediate bits: %d\n", immediate_bits);
    printf("RD bits: %d\n", rd_bits);

    // Open instructions file
    FILE *instructions_file = fopen("instructions.txt", "r");
    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return;
    }

    char instr_line[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH], funct3[MAX_LINE_LENGTH];

    // Find the corresponding opcode and funct3 for the instruction mnemonic
    while (fgets(instr_line, sizeof(instr_line), instructions_file) != NULL) {
        char instr_mnemonic[MAX_LINE_LENGTH];
        if (sscanf(instr_line, "%*s %s %*s %s %s", instr_mnemonic, opcode, funct3) != 3) {
            continue;
        }
        if (strcmp(instr_mnemonic, instruction_mnemonic) == 0) {
            break;
        }
    }

    fclose(instructions_file);

    // Convert immediate value to binary string
    char immediate_binary[13]; // 12 bits + null terminator
    sprintf(immediate_binary, "%012d", immediate_bits);

    // Convert rd to binary string
    char rd_binary[5]; // 4 bits + null terminator
    sprintf(rd_binary, "%04d", rd_bits);

    // Convert operands to binary strings
    char *rs2_binary = decimal_to_binary(rs2);
    char *rs1_binary = decimal_to_binary(rs1);

    // Generate machine code
    char machine_code[33]; // 32 bits + null terminator
    sprintf(machine_code, "%s%s%s%s%s", immediate_binary, rs2_binary, rs1_binary, opcode, funct3);

    // Insert rd bits into machine code at appropriate position
    machine_code[7] = rd_binary[0];
    machine_code[8] = rd_binary[1];
    machine_code[9] = rd_binary[2];
    machine_code[10] = rd_binary[3];

    printf("Machine code (binary): %s\n", machine_code);

    // Convert binary machine code to hexadecimal
    char hex_machine_code[9]; // 8 hex digits + null terminator
    sprintf(hex_machine_code, "%X", (int) strtol(machine_code, NULL, 2));

    // Calculate the program counter (assuming each instruction occupies 4 bytes)
    char pc_hex[9]; // 32 bits / 4 bits per hex digit = 8 digits + null terminator
    sprintf(pc_hex, "0x%02X", *program_counter);

    // Write the assembled instruction to output.mc
    FILE *output_file = fopen("output.mc", "a"); // Open output file in append mode
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
