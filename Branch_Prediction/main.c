#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_INSTRUCTIONS 100000
#define MAX_QUEUE_SIZE 1000
typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front; // Index of the front element
    int rear;  // Index of the rear element
} Queue;

void initializeQueue(Queue *queue);
int isEmpty(Queue *queue);
int isFull(Queue *queue);
void enqueue(Queue *queue, int value);
int dequeue(Queue *queue);

typedef struct {
    char *instruction; 
    char *pc;
    int branch;
    int offset;
    //int tag;
    //Queue history;
} Instruction;

typedef struct {
    int tag;
    char pc[10];
    int offset;
    Queue history;
} Branches;

unsigned int hextodecimal(char *hexstring) {
    unsigned int decimal = strtoul(hexstring, NULL, 16);
    if (decimal == 0 && *hexstring != '0') {
        printf("Error: Conversion failed for input '%s'\n", hexstring);
        return -1; 
    }

    return (unsigned int)decimal;
}
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


// Function to simulate the execution of the program
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
            else {
                //printf("\t branch taken\n");
                //printf("\n");
            }
        }
    }
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

void printBranchHistory(Branches *branches, int numBranches);

void always_not_taken(Branches *branches, int numBranches) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("----------------\nBranch Target Buffer for always not taken:\n");
    printf("Program Counter\t\tPredicted Next PC\tHit/Miss\n");

    for (int i = 0; i < numBranches; i++) {
        printf("0x%s\t\t", branches[i].pc);

        Queue *history = &branches[i].history;

        // Calculate the next PC (pc + 4)
        int nextPC = hextodecimal(branches[i].pc) + 4;
        printf("0x%x\t\t", nextPC); // Print next PC in hexadecimal format

        // Check if the branch is always not taken (history queue is empty)
        if (isEmpty(history)) {
            printf("Not Taken\t");
            // Compare with the actual outcome
            if (branches[i].offset == 0) {
                printf("h\n"); // Hit
                correctPredictions++; // If not taken and offset is 0, prediction is correct
                totalBranches++;
            } else {
                printf("m\n"); // Miss
                totalBranches++;
            }
        } else {
            printf("[");
            for (int j = 0; j <= MAX_QUEUE_SIZE; j++) {
                int index = (history->front + j) % MAX_QUEUE_SIZE;
                if (index == history->rear+1|| index == -1)
                    break;
                if (history->data[index] == 0) {
                    printf("H"); // Hit
                    correctPredictions++;
                    totalBranches++;
                } else {
                    printf("M"); // Miss
                    totalBranches++;
                }
            }
            printf("]\n");
        }
        
        
    }

    // Calculate accuracy of the always not taken branch predictor
    float accuracy = (float)correctPredictions / totalBranches * 100;

    printf("\nAccuracy of Always Not Taken Branch Predictor: %.2f%%\n", accuracy);
}

void always_taken(Branches *branches, int numBranches) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("\n----------------\nBranch Target Buffer for always taken:\n");
    printf("Program Counter\t\tPredicted Next PC\tHit/Miss\n");

    for (int i = 0; i < numBranches; i++) {
        printf("0x%s\t\t", branches[i].pc);

        Queue *history = &branches[i].history;

        // Calculate the next PC (pc + 4)
        int nextPC = hextodecimal(branches[i].pc) + branches[i].offset ;
        printf("0x%x\t\t", nextPC); // Print next PC in hexadecimal format

        // Check if the branch is always not taken (history queue is empty)
        if (isEmpty(history)) {
            printf("Not Taken\t");
            // Compare with the actual outcome
            if (branches[i].offset == 0) {
                printf("M\n"); // Hit
                totalBranches++;
            } else {
                printf("H\n"); // Miss
                correctPredictions++;
                totalBranches++;
            }
        } else {
            printf("[");
            for (int j = 0; j <= MAX_QUEUE_SIZE; j++) {
                int index = (history->front + j) % MAX_QUEUE_SIZE;
                if (index == history->rear+1|| index == -1)
                    break;
                if (history->data[index] == 0) {
                    printf("M"); // Hit
                    totalBranches++;
                } else {
                    printf("H"); // Miss
                    correctPredictions++;
                    totalBranches++;
                }
            }
            printf("]\n");
        }
        
        
    }

    // Calculate accuracy of the always not taken branch predictor
    float accuracy = (float)correctPredictions / totalBranches * 100;

    printf("\nAccuracy of Always Taken Branch Predictor: %.2f%%\n", accuracy);
}

void one_bit_branch_predictor(Branches *branches, int numBranches) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("\n------------\nBranch Target Buffer for one bit branch predictor:\n");
    printf("Program Counter\t\tHistory\t\tPrediction\t\tHit/Miss\n");

    for (int i = 0; i < numBranches; i++) {
        Queue *history = &branches[i].history;

        // Ignore the first instance (no prediction for the first branch)
        if (!isEmpty(history)) {
            for (int j = history->front; j != history->rear; j = (j + 1) % MAX_QUEUE_SIZE) {
                // Calculate the next PC (pc + 4)
                int nextPC = hextodecimal(branches[i].pc) + 4;
                printf("%s\t\t", branches[i].pc);
                //printf("%x\t\t", nextPC); // Print next PC in hexadecimal format

                // Get the branch outcome from history queue
                int outcome = history->data[j];
                int next_outcome = history->data[j+1];

                // Determine the target address based on the previous outcome
                int targetAddress = (outcome == 1) ? nextPC + branches[i].offset : nextPC;

                // Print history
                printf("%d\t\t", outcome);

                // Print target address
                printf("%x\t\t", targetAddress);

                // Check if the prediction matches the actual outcome
                if (outcome == next_outcome) {
                    printf("h\t\t"); // Hit
                    correctPredictions++;
                } else if (outcome !=next_outcome) {
                    printf("m\t\t"); // miss
                } 

                totalBranches++;

                printf("\n");
            }
        }
    }

    // Calculate accuracy of the 1-bit branch predictor
    float accuracy = (float)correctPredictions / totalBranches * 100;

    printf("\nAccuracy of 1-bit Branch Predictor: %.2f%%\n", accuracy);
}

void two_bit_branch_predictor(Branches *branches, int numBranches) {
    int totalBranches = 0;
    int correctPredictions = 0;

    printf("\n------------\nBranch Target Buffer for two-bit branch predictor:\n");
    printf("Program Counter\t\tHistory\t\tPrediction\t\tHit/Miss\n");

    for (int i = 0; i < numBranches; i++) {
        Queue *history = &branches[i].history;

        // Ignore the first instance (no prediction for the first branch)
        if (!isEmpty(history)) {
            for (int j = history->front; j != history->rear; j = (j + 1) % MAX_QUEUE_SIZE) {
                // Calculate the next PC (pc + 4)
                int nextPC = hextodecimal(branches[i].pc) + 4;
                printf("%s\t\t", branches[i].pc);
                //printf("%x\t\t", nextPC); // Print next PC in hexadecimal format

                // Get the branch outcomes from history queue
                int outcome1 = history->data[j];
                int outcome2 = history->data[j + 1];

                // Determine the prediction based on the history (two-bit counter logic)
                char prediction;
                if (outcome1 == 0 && outcome2 == 0) {
                    prediction = 'n'; // Strongly not taken
                } else if (outcome1 == 0 && outcome2 == 1) {
                    prediction = 'n'; // Weakly not taken
                } else if (outcome1 == 1 && outcome2 == 0) {
                    prediction = 't'; // Weakly taken
                } else if (outcome1 == 1 && outcome2 == 1) {
                    prediction = 't'; // Strongly taken
                }

                // Print history (two-bit counter state)
                printf("%d%d\t\t", outcome1, outcome2);

                // Print prediction
                printf("%c\t\t\t", prediction);

                // Check if the prediction matches the actual outcome
                if (outcome1 == outcome2) {
                    printf("h\t\t"); // Hit
                    correctPredictions++;
                } else {
                    printf("m\t\t"); // Miss
                }

                totalBranches++;

                printf("\n");
            }
        }
    }

    // Calculate accuracy of the two-bit branch predictor
    float accuracy = (float)correctPredictions / totalBranches * 100;

    printf("\nAccuracy of Two-bit Branch Predictor: %.2f%%\n", accuracy);
}


int main() {
    Instruction instructions[MAX_INSTRUCTIONS];
    Branches branches[MAX_QUEUE_SIZE];
    //int numBranches = 0;
    int instructionCount = parsetracefile("trace.txt", instructions);
    if (instructionCount == -1)
        return 1;
   
    simulateExecution(instructions, instructionCount);
    int numBranches= storeBranches(instructions, instructionCount , branches);
    //printf("%d\n", numBranches);
    printBranchHistory(branches, numBranches);
    always_not_taken(branches, numBranches);
    always_taken(branches, numBranches);
    one_bit_branch_predictor(branches, numBranches);
    return 0;
}

void printBranchHistory(Branches *branches, int numBranches) {
    int i, j;

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

void initializeQueue(Queue *queue) {
    queue->front = -1; // Initialize front and rear indices
    queue->rear = -1;
}
// Function to check if the queue is empty
int isEmpty(Queue *queue) {
    return (queue->front == -1 && queue->rear == -1);
}
// Function to check if the queue is full
int isFull(Queue *queue) {
    return ((queue->rear + 1) % MAX_QUEUE_SIZE == queue->front);
}
// Function to enqueue an element into the queue
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
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (isEmpty(queue)) {
        queue->front = 0; // If the queue is empty, set front to 0
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE; // Increment rear circularly
    queue->data[queue->rear] = value; // Enqueue the value
}
// Function to dequeue an element from the queue

