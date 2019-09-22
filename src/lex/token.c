#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../headers/token.h"

Token get_token(char *line, node ReservedWords, node *SymbolTable) {

  static char *f;
  static char line_remaining[72];

  if (strcmp(line_remaining, line) != 0) {
    strcpy(line_remaining, line);
    f = &line_remaining[0];
  }

  Token t = machine(line, ReservedWords, SymbolTable);

  return t;
}

Token machine(char *f, node ReservedWords, node *SymbolTable) {
  // this will need to be changed to whitespace machine
  // then loop while t.str == "UNDEF", cascade through
  // the machines.

  // actually this might need to be more like
  // while(the entire line hasn't been processed...)
  Token t = m_whitespace(f);
  while (strcmp(t.str, "UNDEF") == 0) {
    //t = whitespace(f);
    printf("whitespace...\n");
    f = t.f;
    printf("%s\n", t.str);

    printf("idres...\n");
    t = m_idres(f, ReservedWords, SymbolTable);
    f = t.f;
    printf("%s\n", t.str);

    printf("whitespace...\n");
    t = m_whitespace(f);
    f = t.f;
    printf("%s\n", t.str);

    printf("idres...\n");
    t = m_idres(f, ReservedWords, SymbolTable);
    f = t.f;
    printf("%s\n", t.str);

    printf("catchall...\n");
    t = m_catchall(f);
    f = t.f;
    printf("%s\n", t.str);

    printf("idres...\n");
    t = m_idres(f, ReservedWords, SymbolTable);
    f = t.f;
    printf("%s\n", t.str);

    printf("catchall...\n");
    t = m_catchall(f);
    f = t.f;
    printf("%s\n", t.str);

    printf("whitespace...\n");
    t = m_whitespace(f);
    f = t.f;
    printf("%s\n", t.str);
    
    printf("idres...\n");
    t = m_idres(f, ReservedWords, SymbolTable);
    f = t.f;
    printf("%s\n", t.str);
  }

  // UPDATE F!

  return t;
}

Token m_idres(char *f, node ReservedWords, node *SymbolTable) {

  char *b = f;
  Token t;
  t.str = "UNDEF";
  t.type = -1;

  char *strbuffer;
  if(isalpha(*f)) {
    f++;
    while(isalnum(*f)) {
      f++;
    }

    strbuffer = (char *)malloc(sizeof (f-b));
    strncpy(strbuffer, b, f-b);

    strbuffer[(f-b)] = '\0';
    t.str = (char *)malloc(sizeof strbuffer);
    strncpy(t.str, strbuffer, f-b);
  }
  
  // iterate through reserved words for match
  node p = ReservedWords;
  while(p != NULL) {
    if (strcmp(t.str, p->str) == 0) {
      printf("reserved word: %s\n", t.str);
      t.type = p->type;
      t.attr = p->attr;
    }
    p = p->next;
  }

  // if not reserved word...
  if (t.type < 0) {
    
    // check IDTOOLONG
    if (strlen(strbuffer) > 10) {
      t.type = -1;
      t.attr = 0;
    }

    // else get or insert symbol
    node symbol = getNode(*SymbolTable, t.str);
    if(symbol == NULL) {
      symbol = (node)malloc(sizeof(struct LinkedList));
      symbol->str = t.str;
      symbol->type = 1;
      symbol->attr = 1;
      *SymbolTable = insertNode(*SymbolTable, symbol);
      printf("INSERT ID: %s\n", t.str);
    }
    t.type = symbol->type;
    t.attr = symbol->attr;
  }
  
  //printf("%s %d %d\n", t.str, t.type, t.attr);

  t.f = f;
  return t;  
}

Token m_whitespace(char *f) {
  Token t;
  t.str = "UNDEF";
  
  if(isspace(*f)) {
    f++;
    while(isspace(*f)) {
      f++;
    }
    t.str = "WS";
  }

  t.f = f;
  return t;
}

Token m_catchall(char *f) {
  Token t;
  t.str = "UNDEF";

  switch(*f) {
  case '+':
    t.str = "ADD_OP";
    break;
  case '-':
    t.str = "ADD_OP";
    break;
  case '*':
    t.str = "MULT_OP";
    break;
  case '/':
    t.str = "MULT_OP";
    break;
  case '.':
    f++;
    if (*f == '.') {
      t.str = "ELIPSIS";
    } else {
      f--;
      t.str = "DOT";
    }
    break;
  case ',':
    t.str = "COMMA";
    break;
  case ';':
    t.str = "SEMI_COLON";
    break;
  case '(':
    t.str = "OPEN_PAREN";
    break;
  case ')':
    t.str = "CLOSE_PAREN";
    break;
  case '[':
    t.str = "OPEN_BRACKET";
    break;
  case ']':
    t.str = "CLOSE_BRACKET";
    break;
  default:
    t.str = "CAUGHT";
  }
  
  f++;
  t.f = f;
  return t;
}
