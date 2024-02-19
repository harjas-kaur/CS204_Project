Using a Makefile can simplify the compilation process and make it more automated. Here's how you can create a Makefile for your project:

1. **Create a Makefile**: Create a file named `Makefile` in the parent directory of your project. This file will contain instructions for compiling your code.

2. **Write Makefile Rules**: In the `Makefile`, define rules for compiling each component of your assembler and linking them together.

   ```make
   # Makefile for Assembler

   # Compiler
   CC = gcc

   # Compiler flags
   CFLAGS = -Wall -Wextra -pedantic -std=c11

   # Source directories
   MAIN_DIR = main
   SYNTAX_CHECKER_DIR = syntax_checker
   DIRECTIVE_CHECKER_DIR = directive_checker

   # Source files
   MAIN_SRC = $(MAIN_DIR)/main.c
   SYNTAX_CHECKER_SRC = $(SYNTAX_CHECKER_DIR)/syntax_checker.c
   DIRECTIVE_CHECKER_SRC = $(DIRECTIVE_CHECKER_DIR)/directive_checker.c

   # Object files
   MAIN_OBJ = $(MAIN_SRC:.c=.o)
   SYNTAX_CHECKER_OBJ = $(SYNTAX_CHECKER_SRC:.c=.o)
   DIRECTIVE_CHECKER_OBJ = $(DIRECTIVE_CHECKER_SRC:.c=.o)

   # Executable
   EXECUTABLE = assembler

   # Phony targets
   .PHONY: all clean

   # Default target
   all: $(EXECUTABLE)

   # Compile main.c
   $(MAIN_OBJ): $(MAIN_SRC)
   	$(CC) $(CFLAGS) -c $< -o $@

   # Compile syntax_checker.c
   $(SYNTAX_CHECKER_OBJ): $(SYNTAX_CHECKER_SRC)
   	$(CC) $(CFLAGS) -c $< -o $@

   # Compile directive_checker.c
   $(DIRECTIVE_CHECKER_OBJ): $(DIRECTIVE_CHECKER_SRC)
   	$(CC) $(CFLAGS) -c $< -o $@

   # Link object files
   $(EXECUTABLE): $(MAIN_OBJ) $(SYNTAX_CHECKER_OBJ) $(DIRECTIVE_CHECKER_OBJ)
   	$(CC) $(CFLAGS) $^ -o $@

   # Clean
   clean:
   	rm -f $(EXECUTABLE) $(MAIN_OBJ) $(SYNTAX_CHECKER_OBJ) $(DIRECTIVE_CHECKER_OBJ)
   ```

3. **Explanation**:
   - The Makefile defines compiler (`CC`) and compiler flags (`CFLAGS`).
   - Source directories and source files are specified for each component (`MAIN_DIR`, `SYNTAX_CHECKER_DIR`, `DIRECTIVE_CHECKER_DIR`, etc.).
   - Object files are generated from source files.
   - Phony targets `all` and `clean` are defined. `all` compiles the executable, and `clean` removes generated files.
   - Rules are defined for compiling source files and linking them into an executable.

4. **Run Make Commands**:
   - `make`: This command compiles the entire project and generates the executable.
   - `make clean`: This command removes generated object files and the executable.

With the Makefile in place, you can simply run `make` to compile your code and `make clean` to clean up generated files when needed. This makes the build process more automated and manageable.