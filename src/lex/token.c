#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../headers/token.h"

Token get_token(char *line, node ReservedWords, node SymbolTable) {

  static char *f;
  static char line_remaining[72];

  if (strcmp(line_remaining, line) != 0) {
    strcpy(line_remaining, line);
    f = &line_remaining[0];
  }

  Token t = machine(line, ReservedWords, SymbolTable);
  /* if(strcmp(t.str, "UNDEF") != 0) { */
  /*   printf("%s\n", t.str);     */
  /* } */

  return t;
}

Token machine(char *f, node ReservedWords, node SymbolTable) {
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
  }
  

  // UPDATE F!


  return t;
}

Token m_idres(char *f, node ReservedWords, node SymbolTable) {

  char *b = f;
  Token t;
  t.str = "UNDEF";

  if(isalpha(*f)) {
    f++;
    while(isalnum(*f)) {
      f++;
    }

    char strbuffer[10];
    strncpy(strbuffer, b, f-b);

    strbuffer[(f-b)] = '\0';
    t.str = (char *)malloc(sizeof strbuffer);
    strncpy(t.str, strbuffer, f-b);
  }
  

  printf("\n\nchecking reserved words...\n\n");

  // fuck this, ReservedWords needs to be a linkedlist!
  /* while(rw < end) { */
  /*   printf("%s\n", rw->str); */
  /*   rw++; */
  /* } */

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
