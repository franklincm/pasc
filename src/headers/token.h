#include "./reserved.h"
#include "./linkedlist.h"

typedef struct Token{
  char *str;
  int type;
  int attr;
  char *f;
} Token;

Token get_token(char * line, ReservedWord *ReservedWords, strnode SymbolTable);
Token machine(char * f, ReservedWord *ReservedWords, strnode SymbolTable);
Token m_idres(char *f, ReservedWord *ReservedWords, strnode SymbolTable);
Token m_whitespace(char *f);
Token m_catchall(char *f);
