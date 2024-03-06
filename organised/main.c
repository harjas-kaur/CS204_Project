#include <stdio.h>
#include "error.h"
//test file for error checking
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int result = check_errors(filename);

    return result;
}
