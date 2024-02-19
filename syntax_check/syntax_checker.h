#ifndef SYNTAX_CHECKER_H
#define SYNTAX_CHECKER_H

#include <stdio.h>

int check_syntax(FILE *file);
int check_for_stray_commas(FILE *file);
void skip_until_text_directive(FILE *file);

#endif /* SYNTAX_CHECKER_H */
