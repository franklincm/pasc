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
  
  Token t;
  t = machine(f, ReservedWords, SymbolTable);
  f = t.f;

  return t;
}

Token machine(char *f, node ReservedWords, node *SymbolTable) {
  // this will need to be changed to whitespace machine
  // then loop while t.str == "UNDEF", cascade through
  // the machines.

  // actually this might need to be more like
  // while(the entire line hasn't been processed...)
  
  Token t = m_whitespace(f);
  f = t.f;
  while(strlen(f) > 0) {
    t = m_idres(f, ReservedWords, SymbolTable);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }

    t = m_longreal(f);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }

    t = m_real(f);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }
    
    t = m_int(f);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }

    t = m_relops(f);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }

    t = m_catchall(f);
    f = t.f;
    if (strcmp(t.str, "UNDEF") != 0) {
      break;
    }
    
    /* t = m_whitespace(f); */
    /* f = t.f; */
    /* if (strcmp(t.str, "UNDEF") != 0) { */
    /*   break; */
    /* } */
    
    //fputs(f, stdout);
    //printf("%d\n", strlen(f));  
  }
  
  return t;

}

Token m_idres(char *f, node ReservedWords, node *SymbolTable) {

  char *b = f;
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 0;

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
      t.type = p->type;
      t.attr = p->attr;
      //printf("%d %d\n", p->type, p->attr);
      //printf("reserved word: %s %d %d\n", t.str, t.type, t.attr);
      break;
    }
    p = p->next;
  }

  // if not reserved word...
  if (t.type < 0 && strcmp(t.str, "UNDEF") != 0) {
    
    // check IDTOOLONG
    if (strlen(t.str) > 10) {
      //printf("IDTOOLONG");
      t.type = LEXERR_IDTOOLONG;
      t.attr = 0;
    }

    // else get or insert symbol
    node symbol = getNode(*SymbolTable, t.str);
    if(symbol == NULL) {
      symbol = (node)malloc(sizeof(struct LinkedList));
      symbol->str = t.str;
      symbol->type = TOKEN_ID;
      //symbol->attr = 1;
      *SymbolTable = insertNode(*SymbolTable, symbol);
      //printf("INSERT ID: %s\n", t.str);
    }

    int loc = 0;
    node p = *SymbolTable;
    while(p != NULL) {
      if(strcmp(p->str, t.str) == 0) {
        break;
      }
      p = p->next;
      loc++;
    }

    t.type = symbol->type;
    t.attr = loc;
  }
  
  t.f = f;
  return t;  
}

Token m_longreal(char *f) {
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 0;
  char *b = f;
  if(isdigit(*f)) {
    for(int i = 0; i < 5; i++) {
      if (isdigit(*f)) {
        f++;          
      } 
    }
    if(*f == '.') {
      f++;
      for(int i = 0; i < 5; i++) {
        if (isdigit(*f)) {
          f++;
        }
      }
      if(*f == 'E') {
        f++;
        if(isdigit(*f)) {
          int exp_len = 0;
          while(isdigit(*f)) {
            f++;
            exp_len++;
          }
          if (exp_len <= 2) {
            t.str = "LONGREAL";
            t.type = TOKEN_LONGREAL;
            t.attr = 0;
            t.f = f;
            return t;
          }
        }
      } else {
        t.f = b;
        return t;
      }
    }
    
  }
  t.f = f;
  return t;
}

Token m_real(char *f) {
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 0;

  if(isdigit(*f)) {
    char *x = f;
    while(isdigit(*f)) {
      f++;
    }
    
    if(!(*f == '.')) {

    } else {
      if((f - x) > 5) {
        t.str = "XXTOOLONG";
      }
      
      f++;
      char *y = f;
      while(isdigit(*f)) {
        f++;
      }
      if((f - y) > 5) {
        t.str = "YYTOOLONG";
      }
      t.str = "REAL";
      t.type = TOKEN_REAL;
    }
  }
  
  t.f = f;
  return t;
}

Token m_int(char *f) {
  Token t;
  char *b = f;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 0;

  if (isdigit(*f)) {
    while(isdigit(*f)) {
      f++;
    }

    if ((f - b) > 1 && *b == '0') {
      t.str = "LEADINGZERO";
      t.type = TOKEN_LEXERR;
      t.attr = 0;
    } else if ((f - b) > 10) {
      t.str = "INTTOOLONG";
      t.type = TOKEN_LEXERR;
      t.attr = 0;    
    } else {
      t.str = "INT";
      t.type = TOKEN_INT;
      t.attr = 0;
    }
  }
  t.f = f;
  return t;
}

Token m_whitespace(char *f) {
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 99;
  if(*f == '\t' || *f == '\n' || *f == '\r') {
    f++;
    t.str = "WS";
    t.type = TOKEN_WS;
    t.attr = 1;
    t.f = f;

  } else if(isspace(*f)) {
    f++;
    while(isspace(*f)) {
      f++;
    }
    
    t.str = "WS";
    t.type = TOKEN_WS;
    t.attr = 3;
  }
  
  t.f = f;
  return t;

}

Token m_relops(char *f) {
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  switch(*f) {
  case '=':
    f++;
    t.str = "RELOP";
    t.type = TOKEN_RELOP;
    break;
  case '<':
    f++;
    if (*f ==  '=') {
      t.str = "LTE";
    } else if (*f == '>') {
      t.str = "NE";
    } else {
      f--;
      t.str = "LT";
    }
    t.type = TOKEN_RELOP;
    break;
  case '>':
    f++;
    if (*f == '=') {
      t.str = "GTE";
    } else {
      f--;
      t.str = "GT";
    }
    t.type = TOKEN_RELOP;
    break;
  }
  t.f = f;
  return t;
}

Token m_catchall(char *f) {
  Token t;
  t.str = "UNDEF";
  t.type = TOKEN_LEXERR;
  t.attr = 0;
  //printf("catchall: %c\n", *f);
  switch(*f) {
  case '+':
    t.str = "ADD_OP";
    t.type = TOKEN_OP;
    break;
  case '-':
    t.str = "ADD_OP";
    t.type = TOKEN_OP;
    break;
  case '*':
    t.str = "MULT_OP";
    t.type = TOKEN_OP;
    break;
  case '/':
    t.str = "MULT_OP";
    t.type = TOKEN_OP;
    break;
  case '.':
    f++;
    if (*f == '.') {
      t.str = "ELIPSIS";
      t.type = TOKEN_ELIPSIS;
    } else {
      f--;
      t.str = "DOT";
      t.type = TOKEN_DOT;
    }
    break;
  case ',':
    t.str = "COMMA";
    t.type = TOKEN_COMMA;
    break;
  case ':':
    f++;
    if (*f == '=') {
      t.str = "ASSIGN_OP";
      t.type = TOKEN_ASSIGN;
    } else {
      f--;
      t.str = "COLON";
      t.type = TOKEN_COLON;
    }
    break;
  case ';':
    t.str = "SEMI_COLON";
    t.type = TOKEN_SEMICOLON;
    break;
  case '(':
    t.str = "OPEN_PAREN";
    t.type = TOKEN_LPAREN;
    break;
  case ')':
    t.str = "CLOSE_PAREN";
    t.type = TOKEN_RPAREN;
    break;
  case '[':
    t.str = "OPEN_BRACKET";
    t.type = TOKEN_LBRACKET;
    break;
  case ']':
    t.str = "CLOSE_BRACKET";
    t.type = TOKEN_RBRACKET;
    break;
  /* case '\n': */
  /*   break; */
  /* default: */
  /*   t.str = "CAUGHT"; */
  }
  
  f++;
  t.f = f;
  return t;
}

char *type_to_str(Token t) {
  char * type;

  if (t.type >= 100) {
    type = "TOKEN_RES";
    return type;
  }
  
  switch(t.type) {
  case TOKEN_LEXERR:
    type = "TOKEN_LEXERR";
    break;
  case TOKEN_WS:
    type = "TOKEN_WS";
    break;
  case TOKEN_ID:
    type = "TOKEN_ID";
    break;
  case TOKEN_RELOP:
    type = "TOKEN_RELOP";
    break;
  case TOKEN_OP:
    type = "TOKEN_OP";
    break;
  case TOKEN_COMMA:
    type = "TOKEN_COMMA";
    break;
  case TOKEN_LPAREN:
    type = "TOKEN_LPAREN";
    break;
  case TOKEN_RPAREN:
    type = "TOKEN_RPAREN";
    break;
  case TOKEN_ELIPSIS:
    type = "TOKEN_ELIPSIS";
    break;
  case TOKEN_DOT:
    type = "TOKEN_DOT";
    break;
  case TOKEN_ASSIGN:
    type = "TOKEN_ASSIGN";
    break;
  case TOKEN_COLON:
    type = "TOKEN_COLON";
    break;
  case TOKEN_SEMICOLON:
    type = "TOKEN_SEMICOLON";
    break;
  case TOKEN_LBRACKET:
    type = "TOKEN_LBRACKET";
    break;
  case TOKEN_RBRACKET:
    type = "TOKEN_RBRACKET";
    break;
  case TOKEN_INT:
    type = "TOKEN_INT";
    break;
  case TOKEN_LONGREAL:
    type = "TOKEN_LONGREAL";
    break;
  case TOKEN_REAL:
    type = "TOKEN_REAL";
    break;
  case LEXERR_IDTOOLONG:
    type = "LEXERR_IDTOOLONG";
    break;
  }
  return type;
}
