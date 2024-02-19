#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//by harjas
//first draft not really used later
#define MAX_LINE_LENGTH 256

//this function checks for any syntax errors in the given file.
int check_syntax(FILE *file);
int check_for_stray_commas(FILE *file);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    char *extension = strrchr(filename, '.');
    // we are checking if a .asm file has been passed into the program or not.
    if (extension == NULL || strcmp(extension, ".asm") != 0) {
        printf("Error: Not a .asm file.\n");
        return 1;
    }
    //if unable to open file for various reasons
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return 1;
    }
    //function description given below after main
    int syntax_result = check_syntax(file);
    fclose(file);

    if (syntax_result == 0) {
        printf("No syntax errors found in %s\n", filename);
    }
    //returns syntax result, this will be chained to further codes which will execute only after this returns zero.
    return syntax_result;
}

int check_syntax(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;
    int skip_until_text = 0; // Flag to indicate if we need to skip lines until .text directive
    FILE *instructions_file = fopen("instructions.txt", "r");

    if (instructions_file == NULL) {
        printf("Error: Unable to open instructions file\n");
        return 1;
    }

    // Check for stray commas
    error_count += check_for_stray_commas(file);

    // Loop through the lines in the file
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        char *token = strtok(line, " ,\t\n");

        if (token != NULL && token[0] != '#') {
            // Check if we need to skip lines until .text directive
            if (skip_until_text) {
                if (strcmp(token, ".text") == 0) {
                    skip_until_text = 0; // Stop skipping lines
                }
                continue; // Skip this line
            }

            // Check for .data directive
            if (strcmp(token, ".data") == 0) {
                skip_until_text = 1; // Set the flag to start skipping lines
                continue; // Skip this line
            }

            char instruction_name[MAX_LINE_LENGTH];
            strcpy(instruction_name, token);

            // Match the instruction with those in the instructions file
            int found = 0;
            char instr[MAX_LINE_LENGTH];
            int expected_arguments;
            while (fscanf(instructions_file, "%s %d", instr, &expected_arguments) != EOF) {
                if (strcmp(instr, instruction_name) == 0) {
                    found = 1;
                    break;
                }
            }

            // Reset file pointer 
            rewind(instructions_file);

            if (!found) {
                printf("Syntax Error: Unsupported instruction '%s' on line %d\n", instruction_name, line_number);
                error_count++;
            } else {
                // Checking number of arguments
                int argument_count = 0;
                while (token != NULL) {
                    token = strtok(NULL, " ,\t\n");
                    argument_count++;
                }
                if (argument_count - 1 != expected_arguments) { // Subtract 1 for the instruction name
                    printf("Syntax Error: Incorrect number of arguments for instruction '%s' on line %d\n", instruction_name, line_number);
                    error_count++;
                }
            }
        }
    }

    fclose(instructions_file);
    return error_count;
}

int check_for_stray_commas(FILE *file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        int len = strlen(line);
        if (len > 1 && line[len - 2] == ',') { // Check if second last character is comma
            printf("Syntax Error: Stray comma found at the end of line %d\n", line_number);
            error_count++;
        }
        if (feof(file)) // Check for end of file
            break;
    }

    rewind(file);
    return error_count;
}
void skip_until_text_directive(FILE *file) {
    char line[MAX_LINE_LENGTH];

    // Loop through the lines until .text directive is found
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char *token = strtok(line, " ,\t\n");
        if (token != NULL && strcmp(token, ".text") == 0) {
            // .text directive encountered, break the loop
            break;
        }
    }
}
