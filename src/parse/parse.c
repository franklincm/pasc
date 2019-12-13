#include <stdio.h>

#ifndef OUTPUT
#define OUTPUT
#include "../headers/output.h"
#endif

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

#ifndef PARSE
#define PARSE
#include "../headers/parse.h"
#endif


Token get_tok(struct state s) {
  Token t = get_token(s.source,
                      s.listing,
                      s.tokenfile,
                      s.reserved_words,
                      s.symbol_table);
  while(t.type == TOKEN_WS) {
    t = get_token(s.source,
                  s.listing,
                  s.tokenfile,
                  s.reserved_words,
                  s.symbol_table);
  }
  return t;
}

void parse(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           node reserved_words,
           node *symbol_table) {

  struct state s = {source, listing, tokenfile, reserved_words, symbol_table};
  
  Token t = get_tok(s);
  while(t.type != TOKEN_EOF) {
    printf("%s\n", type_to_str(t.type));
    t = get_tok(s);
  }
}

Token match(int token_type, Token t, struct state s) {
  printf("match: %s\n", type_to_str(t.type));
  if (t.type == token_type) {
    t = get_tok(s);
    if (token_type == TOKEN_EOF) {
      printf("END OF PARSE\n");
    } else {
      return t;
    }
  } else {
    printf("Expecting: %s, Got: %s\n", type_to_str(token_type), type_to_str(t.type));
    printf("SYNTAX ERROR\n");
  }
}

void parse_test(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           node reserved_words,
           node *symbol_table) {

  struct state s = {source, listing, tokenfile, reserved_words, symbol_table};
  
  static Token t;
  t = get_tok(s);
  parse_program(t, s);
}

Token parse_program(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = parse_program_start(t, s);
    t = parse_program_declarations(t, s);
    //match(TOKEN_EOF, t, s);
  }
}

Token parse_program_start(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_LPAREN, t, s);
    t = parse_identifier_list(t, s);
    t = match(TOKEN_RPAREN, t, s);
    t = match(TOKEN_SEMICOLON, t, s);
  }
  return t;
}

Token parse_program_declarations(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    break;
  }
  return t;
}

Token parse_program_subprogram_declarations(Token t, struct state s) {}

Token parse_compound_statement(Token t, struct state s) {}

Token parse_identifier_list(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_t(t, s);
  }
  return t;
}

Token parse_identifier_list_t(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_t(t, s);
    return t;
    break;
  case TOKEN_RPAREN:
    break;
  }
  return t;
}

Token parse_declarations(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_t(t, s);
    break;
  }
  return t;
}

Token parse_declarations_t(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);    
    t = parse_type(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_t(t, s);
    break;
  case TOKEN_FUNCTION:
    break;
  case TOKEN_BEGIN:
    break;
  }
  return t;
}

Token parse_type(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_ARRAY:
    t = match(TOKEN_ARRAY, t, s);
    t = match(TOKEN_LBRACKET, t, s);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_ELIPSIS, t, s);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_RBRACKET, t, s);
    t = match(TOKEN_OF, t, s);
    t = parse_standard_type(t, s);
    break;
  case TOKEN_INTEGER:
    t = match(TOKEN_INTEGER, t, s);
    break;
  case TOKEN_RREAL:
    t = match(TOKEN_RREAL, t, s);
    break;
  }
  return t;
}

Token parse_standard_type(Token t, struct state s) {
  switch(t.type) {
  case TOKEN_INTEGER:
    t = match(TOKEN_INTEGER, t, s);
    break;
  case TOKEN_RREAL:
    t = match(TOKEN_RREAL, t, s);
    break;
  }
  return t;
}
