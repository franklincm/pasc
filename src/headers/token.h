#include "./reserved.h"
#include "./linkedlist.h"

typedef struct Token{
  char *str;
  char *type;
  int attr;
  char *f;
} Token;

Token get_token(char * line, ReservedWord * reserved_words, strnode SymbolTable);
