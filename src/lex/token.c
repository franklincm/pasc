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

  // digit
  if(isdigit(*f)) {
    while(isdigit(*f)) f++;
    intbuffer = (char *)malloc(sizeof (f-start));
    strncpy(intbuffer, start, f-start);
    intbuffer[(f-start)] = '\0';

    // dot
    if(*f == '.') {
      f++;
      dec = f;

      // digit
      if(isdigit(*f)) {
        while(isdigit(*f)) f++;
        fracbuffer = (char *)malloc(sizeof(f-dec));
        strncpy(fracbuffer, dec, f-dec);
        fracbuffer[(f-dec)] = '\0';

        // optional 'E'
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
          } else {
            // if 'E', exponent required!
            t.type = LEXERR;
            t.attr = MISSINGEXPONENT;
          }
        }

        // update t.str with lexeme
        char *strbuffer = (char *)malloc(sizeof(f-start));
        strncpy(strbuffer, start, f-start);
        t.str = strbuffer;

        // check for extra long frac
        if(strlen(fracbuffer) > 5) {
          t.type = LEXERR;
          t.attr = FRACTOOLONG;

          // check trailing zero
        } else if (strlen(fracbuffer) > 1 &&
                   fracbuffer[strlen(fracbuffer) - 1] == '0') {
          t.type = LEXERR;
          t.attr = TRAILINGZERO;
        }
      }

      // check extra long int part...
      if(strlen(intbuffer) > 5) {
        t.type = LEXERR;
        t.attr = DIGITTOOLONG;

        // check leading zero...
      } else if (strlen(intbuffer) > 1 && intbuffer[0] == '0') {
        t.type = LEXERR;
        t.attr = LEADINGZERO;
      }

      // if no errors, then assign to real
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
  char *strbuffer = (char *)malloc(100 * sizeof(char));
  
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
      t.type = TOKEN_INT;
    }
    strncpy(strbuffer, b, f-b);
    strbuffer[(f-b)] = '\0';
    t.str = strbuffer;

    if(t.type == TOKEN_INT) {
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

  if(isspace(*f)) {
    while(isspace(*f)) {
      if(*f == '\n') {
        t.str = "EOL";
        t.type = TOKEN_WS;
        t.f = f;
        return t;
      } else {
        f++;
      }
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
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;
  
  switch(*f) {
  case '+':
    t.str = "+";
    t.type = TOKEN_ADDOP;
    break;
  case '-':
    t.str = "-";
    t.type = TOKEN_ADDOP;
    break;
  case '*':
    t.str = "*";
    t.type = TOKEN_MULOP;
    break;
  case '/':
    t.str = "/";
    t.type = TOKEN_MULOP;
    break;
  case '.':
    f++;
    if (*f == '.') {
      t.str = "..";
      t.type = TOKEN_ELIPSIS;
    } else {
      f--;
      t.str = ".";
      t.type = TOKEN_DOT;
    }
    break;
  case ',':
    t.str = ",";
    t.type = TOKEN_COMMA;
    break;
  case ':':
    f++;
    if (*f == '=') {
      t.str = ":=";
      t.type = TOKEN_ASSIGN;
    } else {
      f--;
      t.str = ":";
      t.type = TOKEN_COLON;
    }
    break;
  case ';':
    t.str = ";";
    t.type = TOKEN_SEMICOLON;
    break;
  case '(':
    t.str = "(";
    t.type = TOKEN_LPAREN;
    break;
  case ')':
    t.str = ")";
    t.type = TOKEN_RPAREN;
    break;
  case '[':
    t.str = "[";
    t.type = TOKEN_LBRACKET;
    break;
  case ']':
    t.str = "]";
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

  switch(t.type) {

  case 100:
    type = "PROG";
    break;
  case 101:
    type = "VAR";
    break;
  case 102:
    type = "ARR";
    break;
  case 103:
    type = "OF";
    break;
  case 104:
    type = "INT";
    break;
  case 105:
    type = "REAL";
    break;
  case 106:
    type = "FUNC";
    break;
  case 107:
    type = "PROC";
    break;
  case 108:
    type = "BEGN";
    break;
  case 109:
    type = "END";
    break;
  case 110:
    type = "IF";
    break;
  case 111:
    type = "THEN";
    break;
  case 112:
    type = "ELSE";
    break;
  case 113:
    type = "WHLE";
    break;
  case 114:
    type = "DO";
    break;
  case 115:
    type = "NOT";
    break;
  case 116:
    //type = "OR";
    type = "ADDOP";    
    break;
  case 117:
    //type = "DIV";
    type = "MULOP";    
    break;
  case 118:
    //type = "MOD";
    type = "MULOP";
    break;
  case 119:
    //type = "AND";
    type = "MULOP";
    break;
    
  case LEXERR:
    type = "LEXERR";
    break;
  case TOKEN_WS:
    type = "TOKEN_WS";
    break;
  case TOKEN_ID:
    type = "ID";
    break;
  case TOKEN_ADDOP:
    type = "ADDOP";
    break;
  case TOKEN_MULOP:
    type = "MULOP";
    break;
  case TOKEN_RELOP:
    type = "RELOP";
    break;
  case TOKEN_OP:
    type = "OP";
    break;
  case TOKEN_COMMA:
    type = "COMMA";
    break;
  case TOKEN_LPAREN:
    type = "LPAREN";
    break;
  case TOKEN_RPAREN:
    type = "RPAREN";
    break;
  case TOKEN_ELIPSIS:
    type = "ELIPSIS";
    break;
  case TOKEN_DOT:
    type = "DOT";
    break;
  case TOKEN_ASSIGN:
    type = "ASSIGN";
    break;
  case TOKEN_COLON:
    type = "COLON";
    break;
  case TOKEN_SEMICOLON:
    type = "SEMICOLON";
    break;
  case TOKEN_LBRACKET:
    type = "LBRACKET";
    break;
  case TOKEN_RBRACKET:
    type = "RBRACKET";
    break;
  case TOKEN_INT:
    type = "INT";
    break;
  case TOKEN_REAL:
    type = "REAL";
    break;
  case TOKEN_EOF:
    type = "EOF";

  }
  return type;
}

char *attr_to_str(Token t) {
  char * type;
  
  switch(t.attr) {
  case UNK_SYMBOL:
    type = "Unrecognized Symbol";
    break;
  case 0:
    type = "NULL";
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
    break;
  case MISSINGEXPONENT:
    type = "Missing Exponent";
  }
  return type;
}
