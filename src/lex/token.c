#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../headers/token.h"

Token get_token(char *line, ReservedWord *ReservedWords, strnode SymbolTable) {

  static char *f;
  static char l[72];
  Token t;
  t.type = NULL;

  if (strcmp(l, line) != 0) {
    strcpy(l, line);
    f = &l[0];
  }

  t = machine(line, ReservedWords, SymbolTable);
  if(strcmp(t.str, "UNDEF") != 0) {
    printf("%s\n", t.str);    
  }

  return t;
}

Token machine(char *f, ReservedWord *ReservedWords, strnode SymbolTable) {

  char *b = f;
  Token t;
  t.str = "UNDEF";

  
  
  //idres
  if(isalpha(*f)) {
    f++;
    while(isalnum(*f)) {
      f++;
    }

    char strbuffer[10];
    strncpy(strbuffer, b, f-b);

    strbuffer[(f-b)] = '\0';
    //printf("%s: %d - %d\n", strbuffer, strlen(strbuffer), f-b);
    //printf("str: %s, *f: %c\n", strbuffer, *f);
    t.str = (char *)malloc(sizeof strbuffer);
    strncpy(t.str, strbuffer, f-b);
    //t.str = strbuffer;
    
  }
  

  return t;
}
