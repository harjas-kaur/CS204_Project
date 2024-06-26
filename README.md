# CS204_Project
Risc-v assembler
built on WSL2 (Ubuntu)
# INSTRUCTIONS FOR ASSEMBLER
# note to users

1) instructions.txt contains the list of supported instructions, with their type and number of arguments required.
2) test.asm is a test file used to check the working of the codes
3) To run the program:
    1. Clone from GitHub.
    2. Open a terminal in the "organized" directory.
    3. Execute the compilation script by entering the following command:
            `bash compile.sh`
    4. The file to be assembled is `test.asm`.
    5. The output will be reflected in `output.mc`.
# Description
    main instructions for compiling 
    
    ```console
    gcc -w labels.c error.c data.c 
    ./a.out
    gcc -w -o compile main.c text.c
    ./compile test.asm
    ```
    *-w flag is used just to make compilation neater. there are numerous warnings due to extensive use of string functions and libraries.

    
    
# Explanations
* .txt files
    `_directives.txt_`:contains the list of supported directives for data along with the spapce occupied by each element of that type.
    `_instructions.txt_:` contains the list of supported instructions along with type, opcode, funct3 func7 etc.
* .h files
    `_error.h_` and `_riscv_compiler.h_` link the files together.
* .c files
    `*error.c*` primarily chekcs for syntax errors in the given test.asm file. However it does not highlight if register operands are gven wring in themselves.
    `*data.c*` primarity deals with the assembly of the data segment of the code and stores the data in appropriate memory locations
    `*labels.c*` is a bit of a multitasker. it calls in functions from error.c and data.c, creates label vs pc address table and then removes the text segment as the meaningful information has already been extracted. Then it removes the label names and in the SB instructions replaces the label names with the pc address.
    `*text.c*` has all the functions for assembling instructions, sorted by their types.
    `*main.c*` calls in functions from data.c and others to finally assemble the text segment of the code.

# shortcomings
* syntax checking can be improved.
* the original file is modified during the compilation. it is later resoted and taken care of in the script which   compiles all the stuff.
* bulky code.
# INSTRUCTIONS FOR BRANCH PREDICTOR
the final branch predictor code is in the folder "old". Compile main.c with the assembler trace in trace.txt. Due to array use it can work on a maximum of around 20,000 lines. By using compiler flags it can be extended upto 80,000. Better Algorithms can be used.
