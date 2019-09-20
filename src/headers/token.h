#include "./reserved.h"
#include "./linkedlist.h"

typedef struct Token{
  char *str;
  char *type;
  int attr;
  char *f;
} Token;

Token get_token(char * line, ReservedWord *ReservedWords, strnode SymbolTable);
Token machine(char * f, ReservedWord *ReservedWords, strnode SymbolTable);
