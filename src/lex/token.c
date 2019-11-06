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

int is_unrec(Token t) {
  if (t.type == TOKEN_UNRECOGNIZED_SYMBOL) {
    return 1;
  }
  return 0;
}

Token machine(char *f, node ReservedWords, node *SymbolTable) {
  Token t;
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;

  t = m_whitespace(f);
  if(!is_unrec(t)) return t;
  
  t = m_idres(f, ReservedWords, SymbolTable);
  if(!is_unrec(t)) return t;

  t = m_real(f);    
  if(!is_unrec(t)) return t;

  t = m_int(f);
  if(!is_unrec(t)) return t;

  t = m_relops(f);
  if(!is_unrec(t)) return t;

  t = m_catchall(f);
  return t;
}

Token m_idres(char *f, node ReservedWords, node *SymbolTable) {
  char *b = f;
  Token t;
  //t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
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
  } else {
    return t;
  }
  
  // iterate through reserved words for match
  node p = ReservedWords;
  while(p != NULL) {
    if (strcmp(t.str, p->str) == 0) {
      t.type = p->type;
      t.attr = p->attr;
      t.f = f;
      return t;
      //break;
    }
    p = p->next;
  }

  // if not reserved word...
  if (t.type == TOKEN_UNRECOGNIZED_SYMBOL) {
    
    // check IDTOOLONG
    if (strlen(t.str) > 10) {
      t.type = LEXERR;
      t.attr = IDTOOLONG;
      t.f = f;
      return t;
    }

    // else get or insert symbol
    node symbol = getNode(*SymbolTable, t.str);
    if(symbol == NULL) {
      symbol = (node)malloc(sizeof(struct LinkedList));
      symbol->str = t.str;
      symbol->type = TOKEN_ID;
      *SymbolTable = insertNode(*SymbolTable, symbol);
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

Token m_real(char *f) {
  Token t;
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;

  char *start, *dec, *exp;
  start = f;

  char *intbuffer;
  char *fracbuffer;
  char *expbuffer;
  
  if(isdigit(*f)) {
    while(isdigit(*f)) f++;
    intbuffer = (char *)malloc(sizeof (f-start));
    strncpy(intbuffer, start, f-start);
    intbuffer[(f-start)] = '\0';

    if(*f == '.') {
      f++;
      dec = f;

      if(isdigit(*f)) {
        while(isdigit(*f)) f++;
        fracbuffer = (char *)malloc(sizeof(f-dec));
        strncpy(fracbuffer, dec, f-dec);
        fracbuffer[(f-dec)] = '\0';

        if(*f == 'E') {
          f++;
          exp = f;

          if(isdigit(*f)) {
            while(isdigit(*f)) f++;
            expbuffer = (char *)malloc(sizeof(f-exp));
            strncpy(expbuffer, exp, f-exp);
            expbuffer[(f-exp)] = '\0';

            if(strlen(expbuffer) > 2) {
              t.type = LEXERR;
              t.attr = EXPONENTTOOLONG;
            }
          }
        }
        if(strlen(fracbuffer) > 5) {
          t.type = LEXERR;
          t.attr = FRACTOOLONG;
        } else if (strlen(fracbuffer) > 1 &&
                   fracbuffer[strlen(fracbuffer) - 1] == '0') {
          t.type = LEXERR;
          t.attr = TRAILINGZERO;
        }
      }
      if(strlen(intbuffer) > 5) {
        t.type = LEXERR;
        t.attr = DIGITTOOLONG;
      } else if (strlen(intbuffer) > 1 && intbuffer[0] == '0') {
        t.type = LEXERR;
        t.attr = LEADINGZERO;
      }
      if(t.type != LEXERR) {
        t.type = TOKEN_REAL;
        t.attr = 0;
      }
    }
  }
  
  t.f = f;
  return t;
  
}

Token m_int(char *f) {
  Token t;
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;

  char *b = f;

  if (isdigit(*f)) {
    while(isdigit(*f)) {
      f++;
    }

    if ((f - b) > 1 && *b == '0') {
      t.type = LEXERR;
      t.attr = LEADINGZERO;
    } else if ((f - b) > 10) {
      t.type = LEXERR;
      t.attr = INTTOOLONG;
    } else {
      // parse str to int
      char *strbuffer = (char *)malloc(sizeof (f-b));
      strncpy(strbuffer, b, f-b);
      strbuffer[(f-b)] = '\0';
      
      t.str = "INT";
      t.type = TOKEN_INT;
      t.attr = atoi(strbuffer);
    }
  }
  t.f = f;
  return t;
}

Token m_whitespace(char *f) {
  Token t;
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;

  if(*f == '\n') {
    f++;
    t.str = "EOL";
    t.type = TOKEN_WS;
  } else if(isspace(*f)) {
    while(isspace(*f)) {
      f++;
    }
    
    t.str = "WS";
    t.type = TOKEN_WS;
  }
  
  t.f = f;
  return t;

}

Token m_relops(char *f) {
  Token t;
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;

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
  //t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;
  
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
  case EOF:
    t.type = TOKEN_EOF;
    break;
  default:
    t.type = LEXERR;
    t.attr = UNK_SYMBOL;
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
  case LEXERR:
    type = "LEXERR";
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
  case TOKEN_EOF:
    type = "TOKEN_EOF";
  }
  return type;
}

char *attr_to_str(Token t) {
  char * type;
  
  switch(t.attr) {
  case UNK_SYMBOL:
    type = "Unrecognized Symbol";
    break;
  case IDTOOLONG:
    type = "ID Too Long";
    break;
  case INTTOOLONG:
    type = "Extra Long Int";
    break;
  case LEADINGZERO:
    type = "Leading Zero";
    break;
  case DIGITTOOLONG:
    type = "Extra Long Int Part";
    break;
  case FRACTOOLONG:
    type = "Extra Long Frac";
    break;
  case TRAILINGZERO:
    type = "Trailing Zero";
    break;
  case EXPONENTTOOLONG:
    type = "Extra Long Exp";
  }
  return type;
}
