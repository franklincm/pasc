#include <stdlib.h>
#include "../headers/token.h"

Token get_token(char * line, ReservedWord * reserved_words, strnode SymbolTable) {
  Token t;
  t.str = "test";
  return t;
}
