#include <stdio.h>
#include "error.h"
//test file for error checking
//for testing gcc -o syntax_checker main.c error.c -std=c99 -Wall -Wextra
//can run compiled like ./syntax_checker test.asm
//issues arise when compiling on wsl. reopen the terminal. it will reflect changes.

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
        printf("do the rest.\n");
        return 0;
    }
    
}
