#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

#ifndef OUTPUT
#define OUTPUT
#include "../headers/output.h"
#endif

char *print_error(Token t) {
  char *line_buffer = (char *)malloc(150 * sizeof(char));
  sprintf(line_buffer, "%-10s%-30s%s", type_to_str(t.type), attr_to_str(t), t.str);
  return line_buffer;
}

Token get_token(FILE *input,
                FILE *listing,
                FILE *tokenfile,
                node ReservedWords) {

  // static vars to keep state
  static FILE *source;
  static FILE *listing_file;
  static FILE *token_file;
  static char *pos;
  static char line_buffer[72];
  static int lineno = 0;

  Token t;

  // if file isn't saved, or position reaches EOL
  if (source != input || *pos == '\n') {
    lineno++;
    source = input;
    listing_file = listing;
    token_file = tokenfile;
    // get next line
    fgets(line_buffer, sizeof line_buffer, source);

    if (feof(source)) {
      t.str = "EOF";
      t.type = TOKEN_EOF;
      t.attr = 0;
      write_line_to_file(token_str(t, lineno), token_file);      
      return t;
    }

    // write token file heading
    if (lineno < 2) {
      write_line_to_file(token_heading(), token_file);      
    }
    // write line to listing
    write_line_to_file(listing_str(lineno, line_buffer), listing_file);
  }

  // get next token
  t = get_token_from_line(line_buffer, ReservedWords);

  // if LEXERR, write to listing
  if (t.type == LEXERR) {
    write_line_to_file(lexerr_str(t), listing_file);
  }

  // write token to tokenfile
  write_line_to_file(token_str(t, lineno), token_file);
  
  // update position
  pos = t.f;

  return t;
}

Token get_token_from_line(char *line, node ReservedWords) {

  static char *f;
  static char line_remaining[72];

  if (strcmp(line_remaining, line) != 0) {
    strcpy(line_remaining, line);
    f = &line_remaining[0];
  }
  
  Token t;
  t = nfa(f, ReservedWords);
  f = t.f;

  return t;
}

int is_unrec(Token t) {
  if (t.type == TOKEN_UNRECOGNIZED_SYMBOL) {
    return 1;
  }
  return 0;
}

Token nfa(char *f, node ReservedWords) {
  Token t;
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;

  t = dfa_whitespace(f);
  if(!is_unrec(t)) return t;
  
  t = dfa_idres(f, ReservedWords);
  if(!is_unrec(t)) return t;

  t = dfa_long_real(f);    
  if(!is_unrec(t)) return t;
  
  t = dfa_real(f);    
  if(!is_unrec(t)) return t;

  t = dfa_int(f);
  if(!is_unrec(t)) return t;

  t = dfa_relops(f);
  if(!is_unrec(t)) return t;

  t = dfa_catchall(f);
  return t;
}

Token dfa_idres(char *f, node ReservedWords) {
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
    /* node symbol = getNode(*SymbolTable, t.str); */
    /* if(symbol == NULL) { */
    /*   symbol = (node)malloc(sizeof(struct LinkedList)); */
    /*   symbol->str = t.str; */
    /*   symbol->type = TOKEN_ID; */
    /*   *SymbolTable = insertNode(*SymbolTable, symbol); */
    /* } */

    /* int loc = 0; */
    /* node p = *SymbolTable; */
    /* while(p != NULL) { */
    /*   if(strcmp(p->str, t.str) == 0) { */
    /*     break; */
    /*   } */
    /*   p = p->next; */
    /*   loc++; */
    /* } */

    /* t.type = symbol->type; */
    /* t.attr = loc; */
    int loc = 0;
    t.type = TOKEN_ID;
    t.attr = loc;
  }
  
  t.f = f;
  return t;  
}

Token dfa_long_real(char *f) {
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

          if((*f == '+') || (*f == '-')) {
            f++;
          }

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

          else {
            // E present but no digit follows? not a real
            return t;
          }
        }

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
      } else {
        // if no digit after decimal, not a long real
        return t;
      }

      // update t.str with lexeme
      char *strbuffer = (char *)malloc(sizeof(f-start));
      strncpy(strbuffer, start, f-start);
      t.str = strbuffer;

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

Token dfa_real(char *f) {
  Token t;
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;
  
  char *intbuffer;
  char *fracbuffer;
  
  char *start, *dec;
  start = f;

  if (isdigit(*f)) {
    while(isdigit(*f)) f++;
    intbuffer = (char *)malloc(sizeof (f-start));
    strncpy(intbuffer, start, f-start);
    intbuffer[(f-start)] = '\0';

    if(*f == '.') {
      f++;
      dec = f;

      if(isdigit(*f)) {
        while(isdigit(*f)) f++;
        fracbuffer = (char *) malloc(sizeof(f-dec));
        strncpy(fracbuffer, dec, f-dec);
        fracbuffer[(f-dec)] = '\0';

        if(strlen(fracbuffer) > 5) {
          t.type = LEXERR;
          t.attr = FRACTOOLONG;
        } else if (strlen(fracbuffer) > 1 && fracbuffer[strlen(fracbuffer) - 1] == '0') {
          t.type = LEXERR;
          t.attr = TRAILINGZERO;
        }
      } else {
        // if not digit after decimal, not a real
        return t;
      }

      // update t.str with lexeme
      char *strbuffer = (char *)malloc(sizeof(f-start));
      strncpy(strbuffer, start, f-start);
      t.str = strbuffer;
      
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

Token dfa_int(char *f) {
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

Token dfa_whitespace(char *f) {
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

Token dfa_relops(char *f) {
  Token t;
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  t.attr = 0;

  switch(*f) {
  case '=':
    f++;
    if (*f == '=') {
      t.str = "==";
    } else {
      t.str = "=";      
    }
    t.type = TOKEN_RELOP;
    f++;
    break;
  case '<':
    f++;
    if (*f ==  '=') {
      t.str = "<=";
    } else if (*f == '>') {
      t.str = "<>";
    } else {
      f--;
      t.str = "<";
    }
    t.type = TOKEN_RELOP;
    f++;
    break;
  case '>':
    f++;
    if (*f == '=') {
      t.str = ">=";
    } else {
      f--;
      t.str = ">";
    }
    t.type = TOKEN_RELOP;
    f++;
    break;
  }
  t.f = f;
  return t;
}

Token dfa_catchall(char *f) {
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

char *type_to_str(int t) {
  char * type;

  switch(t) {

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
    type = "INTEGER";
    break;
  case 105:
    type = "RREAL";
    break;
  case 106:
    type = "FUNC";
    break;
  case 107:
    type = "PROC";
    break;
  case 108:
    type = "BEGIN";
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
  case SYNERR:
    type = "SYNERR";
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
