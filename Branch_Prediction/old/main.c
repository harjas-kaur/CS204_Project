#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_INSTRUCTIONS 100000
#define MAX_QUEUE_SIZE 100

//defining main data structures
typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front; 
    int rear;  
} Queue;

typedef struct {
    char *instruction; 
    char *pc;
    int branch;
    int offset;
    int index;
    bool old_bit; //for dynamic branch predictors
    bool new_bit;
} Instruction;

typedef struct {
    int tag;
    char pc[10];
    int offset;
    Queue history; //for printing the branch history table, not required in the final project.
} Branches;

//basic queue functions
void make_queue(Queue *queue);
void enqueue(Queue *queue, int value);
int empty(Queue *queue);

unsigned int hextodecimal(char *hexstring);

//various functions to manipulate the arrays storing the data
void parse2(Instruction *instructions, int count);
void parse3(Instruction *instructions, int *count);
int parse(const char *filename, Instruction *instructions);

//for debugging
void print_history(Branches *branches, Instruction *instructions, int count);

//main functions for branch predictors
//the flag always taken indicates which kind of branch predictor we want to implement
//those are implemented with ternary operators
void static_predictor(Instruction *instructions, int count, int always_taken) {
    int total = 0;
    int correct = 0;
    //print heading
    printf("----------------\nBranch Target Buffer for %s:\n", always_taken ? "Always Taken" : "Always Not Taken");
    printf("Program Counter\t\tPredicted Branch\tPredicted Target\tActual Branch\t      Hit/Miss\n");

    for (int i = 0; i < count; i++) {
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0||
            strcmp(instructions[i].instruction,"ble")==0||strcmp(instructions[i].instruction,"bltu")==0||
            strcmp(instructions[i].instruction,"bge")==0||strcmp(instructions[i].instruction,"bgeu")==0||
            strcmp(instructions[i].instruction, "beqz") == 0 ||
            strcmp(instructions[i].instruction, "bnez") == 0) {
            int pc_old = hextodecimal(instructions[i].pc);
            int pc_new = hextodecimal(instructions[i + 1].pc);
            int predicted_branch = always_taken ? 1 : 0;
            instructions[i].branch = (pc_old + instructions[i].offset == pc_new) ? 1 : 0;
            int predicted_target = always_taken ? (pc_old + 4) : (pc_old + instructions[i].offset);
            printf("0x%s\t\t\t%s\t\t0x%x\t\t\t%s\t\t", instructions[i].pc, always_taken ? "t" : "nt", predicted_target, instructions[i].branch ? "t" : "nt");
            if (predicted_branch == instructions[i].branch) {
                printf("hit\n");
                correct++;
            } else {
                printf("miss\n");
            }
            total++;
        }
    }
    float accuracy = (float)correct / total * 100;
    printf("\nAccuracy of %s Branch Predictor: %.2f%%\n", always_taken ? "Always Taken" : "Always Not Taken", accuracy);
}

//in main, remove nonbranch instructions is executed first
//for simplicity
void dynamic_predictor(Instruction *instructions, int count, int isTwoBit) {
    int total = 0;
    int correct = 0;

    printf("----------------\nBranch Target Buffer for %s bit branch predictor:\n", isTwoBit ? "two" : "one");
    printf("Program Counter\t\tHistory\t\tPred. Branch\tPred. Target\t\tActual Branch\t\tHit/Miss\n");

    //random values for first bit, has no effect on code
    instructions[0].old_bit = 0;
    instructions[0].new_bit = 0;

    for (int i = 1; i < count ; i++) {
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0||
            strcmp(instructions[i].instruction,"ble")==0||strcmp(instructions[i].instruction,"bltu")==0||
            strcmp(instructions[i].instruction,"bge")==0||strcmp(instructions[i].instruction,"bgeu")==0||
            strcmp(instructions[i].instruction, "beqz") == 0 ||
            strcmp(instructions[i].instruction, "bnez") == 0) {
            int pc_old = hextodecimal(instructions[i - 1].pc);
            int pc_current = hextodecimal(instructions[i].pc);

            // history bits are updates only if current pc is same as previous
            // we had sorted the original array so it works
            if (pc_old == pc_current) {   
                if (isTwoBit) {   // for two-bit predictor 
                    
                    instructions[i].old_bit = instructions[i - 1].new_bit;
                    instructions[i].new_bit = instructions[i - 1].branch;
                    if (i==1){
                        continue; //skip the second one too as we don't have prediction bit for it
                    }
                } else {    // for one-bit predictor
                    instructions[i].old_bit = instructions[i - 1].new_bit;
                    instructions[i].new_bit = instructions[i - 1].branch;
                }
            } else {
                //if it is a new pc, skip it as we don't have a prediction.
                instructions[i].old_bit = 0;
                instructions[i].new_bit = 0;
                continue;
            }

            // predicted branch has value according to one bit or two bit
            //if one bit it wll use new_bit
            //if two bit it will use old_bit
            bool predicted_branch = isTwoBit ? instructions[i].old_bit : instructions[i].new_bit;
            int predicted_target = predicted_branch ? (pc_current + instructions[i].offset) : (pc_current + 4);
            
            //printed both the history bits but calculations are done accordingly
            printf("0x%s\t\t%d%d\t\t%s\t\t0x%x\t\t\t%s\t\t", instructions[i].pc, instructions[i].old_bit, instructions[i].new_bit, predicted_branch ? "T" : "NT", predicted_target, instructions[i].branch ? "T" : "NT");

            if (predicted_branch == instructions[i].branch) {
                printf("Hit\n");
                correct++;
            } else {
                printf("Miss\n");
            }
            total++;
        }
    }

    float accuracy = (float)correct / total * 100;
    printf("\nAccuracy of %s bit Branch Predictor: %.2f%%\n", isTwoBit ? "two" : "one", accuracy);
}


int main() {
    Instruction instructions[MAX_INSTRUCTIONS];
    Branches branches[MAX_QUEUE_SIZE];
    int instructionCount = parse("trace.txt", instructions);
    parse2(instructions, instructionCount);
    if (instructionCount == -1)
        return 1;
      int choice;
    do {
        // Display menu
        printf("\nSelect:\n");
        printf("1. print history\n");
        printf("2. static: always taken\n");
        printf("3. static: always not taken\n");
        printf("4. dynamic: two bit\n");
        printf("5. dynamic: one bit\n");
        printf("6. none\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        // Process user choice
        switch (choice) {
            case 1:
                instructionCount = parse("trace.txt", instructions);
                parse2(instructions, instructionCount);
                print_history(branches, instructions, instructionCount);
                break;
            case 2:
                instructionCount = parse("trace.txt", instructions);
                parse2(instructions, instructionCount);
                static_predictor(instructions, instructionCount, 1); // always taken
                break;
            case 3:
                instructionCount = parse("trace.txt", instructions);
                parse2(instructions, instructionCount);
                static_predictor(instructions, instructionCount, 0); // always not taken
                break;
            case 4:
                parse3(instructions, &instructionCount);
                dynamic_predictor(instructions, instructionCount, 1); // two bit branch predictor
                break;
            case 5:
                parse3(instructions, &instructionCount);
                dynamic_predictor(instructions, instructionCount, 0); // one bit branch predictor
                break;
            case 6:
                printf("finish\n");
                break;
            default:
                printf("enter a number between 1 and 7\n");
        }
    } while (choice != 6);

    
    //user calls them
    return 0;
}
//****************************
// functions to parse the trace file
//returns the number of instructions in the file
int parse(const char *filename, Instruction *instructions) {
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
        instructions[count].pc = strdup(token);
        if (instructions[count].pc == NULL) {
            printf("allocation failed\n");
            fclose(file);
            return -1;
        }
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        instructions[count].instruction = strdup(token);
        if (instructions[count].instruction == NULL) {
            printf("allocation failed\n");
            fclose(file);
            free(instructions[count].pc); 
            return -1;
        }
        if (strcmp(instructions[count].instruction,"beq")==0||strcmp(instructions[count].instruction,"bne")==0||
            strcmp(instructions[count].instruction,"blt")==0||strcmp(instructions[count].instruction,"bltu")==0||
            strcmp(instructions[count].instruction,"bge")==0||strcmp(instructions[count].instruction,"bgeu")==0) {
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
//this checks whether branch was taken and then updates the value in instruction.branch
void parse2(Instruction *instructions, int count) {
    for (int i = 0; i < count; i++) {
        //printf("%s %s\n", instructions[i].instruction, instructions[i].pc);
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0||
            strcmp(instructions[i].instruction,"blt")==0||strcmp(instructions[i].instruction,"bltu")==0||
            strcmp(instructions[i].instruction,"bge")==0||strcmp(instructions[i].instruction,"bgeu")==0||
            strcmp(instructions[i].instruction,"beqz")==0||strcmp(instructions[i].instruction,"bnez")==0) {
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
//this removes all non branch instructions from the array
void parse3(Instruction *instructions, int *count) {
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
            strcmp(instructions[i].instruction, "blt") == 0 || strcmp(instructions[i].instruction, "bltu") == 0 ||
            strcmp(instructions[i].instruction, "bge") == 0 || strcmp(instructions[i].instruction, "bgeu") == 0 ||
            strcmp(instructions[i].instruction, "beqz") == 0 || strcmp(instructions[i].instruction, "bnez") == 0) {
            instructions[new_count++] = instructions[i];
        } else {
            free(instructions[i].pc);
            free(instructions[i].instruction);
        }
    }
    *count = new_count;
}
//*****************************
//functions for printing the history for debugging
int store_branches(Instruction *instructions, int count, Branches *branches) {
    bool isNewBranch;
    int new_count = 0; // Initialize new_count

    for (int i = 0; i < count; i++) {
        // Check if the instruction is a branch instruction
        if (strcmp(instructions[i].instruction,"beq")==0||strcmp(instructions[i].instruction,"bne")==0||
            strcmp(instructions[i].instruction,"blt")==0||strcmp(instructions[i].instruction,"bltu")==0||
            strcmp(instructions[i].instruction,"bge")==0||strcmp(instructions[i].instruction,"bgeu")==0||
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
                make_queue(&branches[new_count].history); // Initialize history queue
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
void print_history(Branches *branches, Instruction *instructions, int count) {
    int i, j;
    int numBranches= store_branches(instructions, count , branches);
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
//*****************************
//functions for queues
void make_queue(Queue *queue) {
    queue->front = -1; // Initialize front and rear indices
    queue->rear = -1;
}
int empty(Queue *queue) {
    return (queue->front == -1 && queue->rear == -1);
}
int full(Queue *queue) {
    return ((queue->rear + 1) % MAX_QUEUE_SIZE == queue->front);
}
int dequeue(Queue *queue) {
    int value;
    if (empty(queue)) {
        printf("queue empty\n");
        exit(1); 
    }
    value = queue->data[queue->front]; 
    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE; 
    }
    return value;
}
void enqueue(Queue *queue, int value) {
    if (full(queue)) {
        dequeue(queue);
    }
    if (empty(queue)) {
        queue->front = 0; 
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->data[queue->rear] = value; 
}
unsigned int hextodecimal(char *hexstring) {
    unsigned int decimal = strtoul(hexstring, NULL, 16);
    if (decimal == 0 && *hexstring != '0') {
        printf("error:for '%s'\n", hexstring);
        return -1; 
    }

    return (unsigned int)decimal;
}
