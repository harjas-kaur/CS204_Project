#ifndef SYNTAX_CHECKER_H
#define SYNTAX_CHECKER_H

#include <stdio.h>

// Function declarations for the public interface
int check_syntax(FILE *file);
static int check_for_stray_commas(FILE *file);
static void skip_until_text_directive(FILE *file);

#endif // SYNTAX_CHECKER_H
