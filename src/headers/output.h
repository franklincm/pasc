#ifndef TOKEN
#define TOKEN
#include "token.h"
#endif

char *token_heading();
char *token_str();
char *lexerr_str(Token t);
char *listing_str(int lineno, char *line);
void write_line_to_file(char *line, FILE *fp);
char *hr_str(int length, char c);
