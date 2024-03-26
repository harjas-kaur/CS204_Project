#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_INSTRUCTIONS 100000
#define MAX_QUEUE_SIZE 100
typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front; // Index of the front element
    int rear;  // Index of the rear element
} Queue;

typedef struct {
    char *instruction; 
    char *pc;
    int branch;
    int offset;
    int index;
    bool old_bit;
    bool new_bit;
} Instruction;

typedef struct {
    int tag;
    char pc[10];
    int offset;
    Queue history;
} Branches;

void initializeQueue(Queue *queue);
void enqueue(Queue *queue, int value);
void simulateExecution(Instruction *instructions, int count);
void printBranchHistory(Branches *branches, Instruction *instructions, int count);
void removeNonBranchInstructions(Instruction *instructions, int *count);
unsigned int hextodecimal(char *hexstring);
int isEmpty(Queue *queue);
// function to parse the trace file

int parsetracefile(const char *filename, Instruction *instructions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        char *token = strtok(line, " ");
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        
        // Allocate memory and copy the content of token to instructions[count].pc
        instructions[count].pc = strdup(token);
        if (instructions[count].pc == NULL) {
            printf("Error: Memory allocation failed\n");
            fclose(file);
            return -1;
        }

        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        
        // Allocate memory and copy the content of token to instructions[count].instruction
        instructions[count].instruction = strdup(token);
        if (instructions[count].instruction == NULL) {
            printf("Error: Memory allocation failed\n");
            fclose(file);
            free(instructions[count].pc); // Free allocated memory for pc
            return -1;
        }
        
        if (strcmp(instructions[count].instruction, "beq") == 0 || strcmp(instructions[count].instruction, "bne") == 0) {
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            if (strcmp(token, "-") == 0) {
                token = strtok(NULL, " ");
                instructions[count].offset = -atoi(token);
            } else {
                token = strtok(NULL, " ");
                instructions[count].offset = atoi(token);
            }
        } else if (strcmp(instructions[count].instruction, "beqz") == 0 || strcmp(instructions[count].instruction, "bnez") == 0) {
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            if (strcmp(token, "-") == 0) {
                token = strtok(NULL, " ");
                instructions[count].offset = -atoi(token);
            } else {
                token = strtok(NULL, " ");
                instructions[count].offset = atoi(token);
            }
        } else {
            instructions[count].branch = -1;
            instructions[count].offset = 0;
        }

        count++;
    }

    fclose(file);
    return count;
}

void static_predictor(Instruction *instructions, int count, int alwaysTaken) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("----------------\nBranch Target Buffer for %s:\n", alwaysTaken ? "Always Taken" : "Always Not Taken");
    printf("Program Counter\t\tPredicted Branch\tPredicted Target\tActual Branch\t      Hit/Miss\n");

    for (int i = 0; i < count; i++) {
        if (strcmp(instructions[i].instruction, "beq") == 0 ||
            strcmp(instructions[i].instruction, "bne") == 0 ||
            strcmp(instructions[i].instruction, "beqz") == 0 ||
            strcmp(instructions[i].instruction, "bnez") == 0) {
            int pc_old = hextodecimal(instructions[i].pc);
            int pc_new = hextodecimal(instructions[i + 1].pc);
            
            int predictedBranch = alwaysTaken ? 1 : 0;
            instructions[i].branch = (pc_old + instructions[i].offset == pc_new) ? 1 : 0;

            int predictedTarget = alwaysTaken ? (pc_old + 4) : (pc_old + instructions[i].offset);

            printf("0x%s\t\t\t%s\t\t0x%x\t\t\t%s\t\t", instructions[i].pc, alwaysTaken ? "T" : "NT", predictedTarget, instructions[i].branch ? "T" : "NT");
            
            // Check if the prediction matches the actual branch
            if (predictedBranch == instructions[i].branch) {
                printf("Hit\n");
                correctPredictions++;
            } else {
                printf("Miss\n");
            }
            totalBranches++;
        }
    }

    // Calculate accuracy
    float accuracy = (float)correctPredictions / totalBranches * 100;
    printf("\nAccuracy of %s Branch Predictor: %.2f%%\n", alwaysTaken ? "Always Taken" : "Always Not Taken", accuracy);
}

void dynamic_predictor(Instruction *instructions, int count, int isTwoBit) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("----------------\nBranch Target Buffer for %s bit branch predictor:\n", isTwoBit ? "two" : "one");
    printf("Program Counter\t\tHistory\t\tPred. Branch\tPred. Target\t\tActual Branch\t\tHit/Miss\n");

    // Initialize history bits for the first instruction
    instructions[0].old_bit = 0;
    instructions[0].new_bit = 0;

    for (int i = 1; i < count ; i++) {
        if (strcmp(instructions[i].instruction, "beq") == 0 ||
            strcmp(instructions[i].instruction, "bne") == 0 ||
            strcmp(instructions[i].instruction, "beqz") == 0 ||
            strcmp(instructions[i].instruction, "bnez") == 0) {
            int pc_old = hextodecimal(instructions[i - 1].pc);
            int pc_current = hextodecimal(instructions[i].pc);

            // Update the history bits if the previous PC is the same as the current one
            if (pc_old == pc_current) {
                if (isTwoBit) {
                    // For two-bit predictor
                    instructions[i].old_bit = instructions[i - 1].new_bit;
                    instructions[i].new_bit = instructions[i - 1].branch;
                    if (i==1){
                        continue;
                    }
                } else {
                    // For one-bit predictor
                    instructions[i].old_bit = instructions[i - 1].new_bit;
                    instructions[i].new_bit = instructions[i - 1].branch;
                }
            } else {
                // Reset history bits to 0 if the previous PC is different
                continue;
            }

            // Predict the branch based on the predictor type
            bool predictedBranch = isTwoBit ? instructions[i].old_bit : instructions[i].new_bit;

            // Calculate the predicted target
            int predictedTarget = predictedBranch ? (pc_current + instructions[i].offset) : (pc_current + 4);

            // Print the results
            printf("0x%s\t\t%d%d\t\t%s\t\t0x%x\t\t\t%s\t\t", instructions[i].pc, instructions[i].old_bit, instructions[i].new_bit, predictedBranch ? "T" : "NT", predictedTarget, instructions[i].branch ? "T" : "NT");

            // Check if the prediction matches the actual branch
            if (predictedBranch == instructions[i].branch) {
                printf("Hit\n");
                correctPredictions++;
            } else {
                printf("Miss\n");
            }

            totalBranches++;
        }
    }

    // Calculate accuracy if there are branches
    if (totalBranches > 0) {
        // Calculate accuracy
        float accuracy = (float)correctPredictions / totalBranches * 100;
        printf("\nAccuracy of %s bit Branch Predictor: %.2f%%\n", isTwoBit ? "two" : "one", accuracy);
    } else {
        printf("\nNo accuracy calculation possible.\n");
    }
}
int main() {
    Instruction instructions[MAX_INSTRUCTIONS];
    Branches branches[MAX_QUEUE_SIZE];
    int instructionCount = parsetracefile("trace.txt", instructions);
    
    if (instructionCount == -1)
        return 1;
   
    simulateExecution(instructions, instructionCount);
    //int numBranches= storeBranches(instructions, instructionCount , branches);
    printBranchHistory(branches, instructions, instructionCount );
    //static_predictor(instructions, instructionCount, 1);
    //static_predictor(instructions, instructionCount, 0);
    removeNonBranchInstructions(instructions, &instructionCount);
    dynamic_predictor(instructions, instructionCount, 1);
    return 0;
}

int storeBranches(Instruction *instructions, int count, Branches *branches) {
    bool isNewBranch;
    int new_count = 0; // Initialize new_count

    for (int i = 0; i < count; i++) {
        // Check if the instruction is a branch instruction
        if (strcmp(instructions[i].instruction, "beq") == 0 ||
            strcmp(instructions[i].instruction, "bne") == 0 ||
            strcmp(instructions[i].instruction, "beqz") == 0 ||
            strcmp(instructions[i].instruction, "bnez") == 0) {

            isNewBranch = true;

            // Check if the program counter (pc) is already in the list of branches
            for (int j = 0; j < new_count; j++) {
                if (strcmp(instructions[i].pc, branches[j].pc) == 0) {
                    isNewBranch = false; // PC already exists in the list
                    break;
                }
            }

            // If it's a new branch, add it to the list
            if (isNewBranch) {
                strcpy(branches[new_count].pc, instructions[i].pc);
                branches[new_count].offset=instructions[i].offset;
                initializeQueue(&branches[new_count].history); // Initialize history queue
                enqueue(&branches[new_count].history, instructions[i].branch); // Add branch outcome to history
                new_count++;
            } else {
                // Find the index of the branch in the branches array
                int branchIndex = -1;
                for (int k = 0; k < new_count; k++) {
                    if (strcmp(instructions[i].pc, branches[k].pc) == 0) {
                        branchIndex = k;
                        break;
                    }
                }
                // Update history queue with branch outcome
                enqueue(&branches[branchIndex].history, instructions[i].branch);
            }
        }
    }
    return new_count;
}
void printBranchHistory(Branches *branches, Instruction *instructions, int count) {
    int i, j;
    int numBranches= storeBranches(instructions, count , branches);
    printf("Branch History Table:\n");
    printf("Program Counter\tHistory\n");

    for (i = 0; i < numBranches; i++) {
        printf("%s\t\t", branches[i].pc);

        Queue *history = &branches[i].history;

        // Print history stored in the queue
        printf("[");
        // Iterate through the queue elements
        for (j = 0; j < MAX_QUEUE_SIZE; j++) {
            // Compute the index with circular wrap-around
            int index = (history->front + j) % MAX_QUEUE_SIZE;
            printf("%d ", history->data[index]);
            // Stop printing if reached the end of the queue
            if (index == history->rear)
                break;
        }
        printf("]\n");
    }
}

void simulateExecution(Instruction *instructions, int count) {
    for (int i = 0; i < count; i++) {
        //printf("%s %s\n", instructions[i].instruction, instructions[i].pc);
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0||strcmp(instructions[i].instruction,"beqz")==0||strcmp(instructions[i].instruction,"bnez")==0) {
            int pc_old=hextodecimal(instructions[i].pc);
            int pc_new=hextodecimal(instructions[i+1].pc);
            //printf("old:%s %s\n", instructions[i].pc, instructions[i+1].pc);
            //printf("converted:%d,%d\n", pc_old, pc_new);
            if ( pc_old+4==pc_new){
                //printf("\tbranch not taken\n");
                instructions[i].branch = 0;
                
            } else if (pc_old +instructions[i].offset==pc_new){ 
                //printf("%d\n",pc_old +instructions[i].offset);
                //printf("\tbranch taken\n");
                instructions[i].branch = 1;
            }
            
        }
    }
    
}

void removeNonBranchInstructions(Instruction *instructions, int *count) {
    // Assign index to each instruction
    for (int i = 0; i < *count; i++) {
        instructions[i].index = i;
    }

    // Sort the array based on PC and index
    for (int i = 0; i < *count - 1; i++) {
        for (int j = 0; j < *count - i - 1; j++) {
            // Compare PCs
            int pc_compare = strcmp(instructions[j].pc, instructions[j + 1].pc);
            if (pc_compare > 0 || (pc_compare == 0 && instructions[j].index > instructions[j + 1].index)) {
                // Swap the instructions
                Instruction temp = instructions[j];
                instructions[j] = instructions[j + 1];
                instructions[j + 1] = temp;
            }
        }
    }

    // Update count
    int new_count = 0;
    for (int i = 0; i < *count; i++) {
        if (strcmp(instructions[i].instruction, "beq") == 0 || strcmp(instructions[i].instruction, "bne") == 0 ||
            strcmp(instructions[i].instruction, "beqz") == 0 || strcmp(instructions[i].instruction, "bnez") == 0) {
            instructions[new_count++] = instructions[i];
        } else {
            free(instructions[i].pc);
            free(instructions[i].instruction);
        }
    }
    *count = new_count;
}

void initializeQueue(Queue *queue) {
    queue->front = -1; // Initialize front and rear indices
    queue->rear = -1;
}
int isEmpty(Queue *queue) {
    return (queue->front == -1 && queue->rear == -1);
}
int isFull(Queue *queue) {
    return ((queue->rear + 1) % MAX_QUEUE_SIZE == queue->front);
}
int dequeue(Queue *queue) {
    int value;
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        exit(1); // Error condition, queue is empty
    }
    value = queue->data[queue->front]; // Dequeue the value
    if (queue->front == queue->rear) {
        // If there was only one element in the queue
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE; // Increment front circularly
    }
    return value;
}
void enqueue(Queue *queue, int value) {
    if (isFull(queue)) {
        dequeue(queue);
    }
    if (isEmpty(queue)) {
        queue->front = 0; // If the queue is empty, set front to 0
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE; // Increment rear circularly
    queue->data[queue->rear] = value; // Enqueue the value
}
unsigned int hextodecimal(char *hexstring) {
    unsigned int decimal = strtoul(hexstring, NULL, 16);
    if (decimal == 0 && *hexstring != '0') {
        printf("Error: Conversion failed for input '%s'\n", hexstring);
        return -1; 
    }

    return (unsigned int)decimal;
}
