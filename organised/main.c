#include <stdio.h>
#include "error.h"
#include "riscv_compiler.h"
//test file for error checking
//for testing gcc -o syntax_checker main.c error.c -std=c99 -Wall -Wextra
//can run compiled like ./syntax_checker test.asm
//issues arise when compiling on wsl. reopen the terminal. it will reflect changes.
void assemble_data_segment(){
    // Open the RISC-V file
    FILE *riscv_file = fopen("test.asm", "r");
    if (riscv_file == NULL) {
        printf("Error: Unable to open RISC-V file\n");
        return;
    }

    // Create or open the output file
    FILE *output_file = fopen("output.mc", "w");
    if (output_file == NULL) {
        printf("Error: Unable to create/open output file\n");
        fclose(riscv_file);
        return ;
    }

    // Compile the data segment
    compile_data_segment(riscv_file, output_file);

    // Close the files
    fclose(riscv_file);
    fclose(output_file);

    return ;
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    //printf("Debug 1\n");

    const char *filename = argv[1];
    int result = check_errors(filename);

    //printf("Debug 2\n");
    //printf("Result: %d\n", result); // Print the result
    if(result!=0){
        printf("Can not assemble! Fix errors!\n");
        return 0;
    } else{
        assemble_data_segment();
        return 0;
    }
    
}
