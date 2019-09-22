#ifndef RESERVED
#define RESERVED

#include "./reserved.h"

#endif

#ifndef LINKED_LIST
#define LINKED_LIST

#include "./linkedlist.h"

#endif

typedef struct Token{
  char *str;
  int type;
  int attr;
  char *f;
} Token;

Token get_token(char * line, node ReservedWords, node *SymbolTable);
Token machine(char * f, node ReservedWords, node *SymbolTable);
Token m_idres(char *f, node ReservedWords, node *SymbolTable);
Token m_whitespace(char *f);
Token m_catchall(char *f);
