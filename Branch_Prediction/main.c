#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_INSTRUCTIONS 1000

// Structure to hold instruction details
typedef struct {
    char instruction[MAX_LINE_LENGTH]; // PC is a hex value, so it can be stored in a string of length 8 + 1 for null terminator
    char pc[9];
    bool branch;
    int offset;
} Instruction;
int hexToDecimal(char *hexString) {
    unsigned int decimalValue = 0;

    // Iterate through each character in the string
    for (int i = 0; hexString[i] != '\0'; i++) {
        char c = hexString[i];

        // Convert hex characters to decimal values
        if (isdigit(c)) {
            decimalValue = decimalValue * 16 + (c - '0');
        } else {
            c = toupper(c);
            decimalValue = decimalValue * 16 + (c - 'A' + 10);
        }
    }

    return decimalValue;
}
// Function to parse the trace file
int parseTraceFile(const char *filename, Instruction *instructions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = 0;

        char *token = strtok(line, " ");
        strcpy(instructions[count].pc, token);

        token = strtok(NULL, " ");
        strcpy(instructions[count].instruction, token);

        // Check if it's a branch instruction
        if (strstr(instructions[count].instruction, "beq") != NULL || strstr(instructions[count].instruction, "bne") != NULL) {
            instructions[count].branch = true;
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            printf("%s\n", token);
            printf("debug\n");
            // Convert the token to an integer value
            //int offset = atoi(token);
            //printf("%d", offset);
            //instructions[count].offset = (unsigned int)offset;
            //printf("%d", instructions[count].offset);

        } else {
            instructions[count].branch = false;
            instructions[count].offset = 0;
        }

        count++;
    }

    fclose(file);
    return count;
}

// Function to simulate the execution of the program
void simulateExecution(const Instruction *instructions, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s %s", instructions[i].instruction, instructions[i].pc);
        
        // Check if it's a branch instruction
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0) {
            int pc_old=hexToDecimal(instructions[i-1].pc);
            int pc_new=hexToDecimal(instructions[i].pc);
            printf("%d,%d", pc_old, pc_new);
            if ( pc_old +4==pc_new){
                printf("branch not taken\n");
            } else /*if (pc_old +instructions[i].offset==pc_new)*/{ 
                printf("\t branch taken\n");
            }
        } else {
            printf("\n");
        }
    }
}

int main() {
    // Read input trace file
    Instruction instructions[MAX_INSTRUCTIONS];
    int instructionCount = parseTraceFile("trace.txt", instructions);
    if (instructionCount == -1)
        return 1;

    // Simulate execution
    simulateExecution(instructions, instructionCount);

    return 0;
}
