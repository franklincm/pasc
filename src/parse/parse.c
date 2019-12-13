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
    fflush(stdout);
    t = get_tok(s);
  }
}

Token match(int token_type, Token t, struct state s) {
  printf("match: %s    %s\n", type_to_str(t.type), t.str);
  fflush(stdout);
  if (t.type == token_type) {
    t = get_tok(s);
    if (token_type == TOKEN_EOF) {
      printf("END OF PARSE\n");
      fflush(stdout);
    } else {
      return t;
    }
  } else {
    printf("Expecting: %s, Got: %s    %s\n", type_to_str(token_type), type_to_str(t.type), t.str);
    fflush(stdout);
    printf("SYNTAX ERROR\n");
    fflush(stdout);
  }
  return t;
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
    t = parse_program_subprogram_declarations(t, s);
    t = parse_compound_statement(t, s);
    t = match(TOKEN_DOT, t, s);
    t = match(TOKEN_EOF, t, s);
  }
  return t;
}

Token parse_program_start(Token t, struct state s) {
  printf("%s\n", "start");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_LPAREN, t, s);
    t = parse_identifier_list(t, s);
    t = match(TOKEN_RPAREN, t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    break;
  }
  return t;
}

Token parse_program_declarations(Token t, struct state s) {
  printf("%s\n", "program_declarations");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    break;
  case TOKEN_FUNCTION:
  case TOKEN_BEGIN:
    printf("%s\n", "program_declarations ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_program_subprogram_declarations(Token t, struct state s) {
  printf("%s\n", "program_subprogram_declarations");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    break;
  case TOKEN_BEGIN:
    printf("%s\n", "program_subprogram_declarations ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_identifier_list(Token t, struct state s) {
  printf("%s\n", "identifier_list");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_t(t, s);
    break;
  }
  return t;
}

Token parse_identifier_list_t(Token t, struct state s) {
  printf("%s\n", "identifier_list_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_t(t, s);
    break;
  case TOKEN_RPAREN:
    printf("%s\n", "identifier_list_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_declarations(Token t, struct state s) {
  printf("%s\n", "declarations");
  fflush(stdout);
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
  printf("%s\n", "declarations_t");
  fflush(stdout);
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
  case TOKEN_BEGIN:
    printf("%s\n", "declarations_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_type(Token t, struct state s) {
  printf("%s\n", "type");
  fflush(stdout);
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
  case TOKEN_RREAL:
    t = parse_standard_type(t, s);
    break;
  }
  return t;
}

Token parse_standard_type(Token t, struct state s) {
  printf("%s\n", "standard_type");
  fflush(stdout);
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

Token parse_subprogram_declarations(Token t, struct state s) {
  printf("%s\n", "subprogram_declarations");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_t(t, s);
    break;
  case TOKEN_BEGIN:
    printf("%s\n", "subprogram_declarations ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_subprogram_declarations_t(Token t, struct state s) {
  printf("%s\n", "subprogram_declarations_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_t(t, s);
    break;
  case TOKEN_BEGIN:
    printf("%s\n", "subprogram_declarations_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_subprogram_declaration(Token t, struct state s) {
  printf("%s\n", "subprogram_declaration");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_head(t, s);
    t = parse_program_declarations(t, s);
    t = parse_program_subprogram_declarations(t, s);
    t = parse_compound_statement(t, s);
    break;
  case TOKEN_SEMICOLON:
    printf("%s\n", "subprogram_declaration ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_subprogram_head(Token t, struct state s) {
  printf("%s\n", "subprogram_head");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = match(TOKEN_FUNCTION, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_subprogram_head_t(t, s);
    break;
  }
  return t;
}

Token parse_subprogram_head_t(Token t, struct state s) {
  printf("%s\n", "subprogram_head_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_COLON:
    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    break;
  case TOKEN_LPAREN:
    t = parse_arguments(t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    break;
  }
  return t;
}

Token parse_arguments(Token t, struct state s) {
  printf("%s\n", "arguments");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_parameter_list(t, s);
    t = match(TOKEN_RPAREN, t, s);
    break;
  }
  return t;
}

Token parse_parameter_list(Token t, struct state s) {
  printf("%s\n", "parameter_list");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    t = parse_parameter_list_t(t, s);
    break;
  }
  return t;
}

Token parse_parameter_list_t(Token t, struct state s) {
  printf("%s\n", "parameter_list_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    t = parse_parameter_list_t(t, s);
    break;
  case TOKEN_RPAREN:
    printf("%s\n", "parameter_list_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_compound_statement(Token t, struct state s) {
  printf("%s\n", "compound_statement");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_BEGIN:
    t = match(TOKEN_BEGIN, t, s);
    t = parse_compound_statement_t(t, s);
    break;
  }
  return t;
}

Token parse_compound_statement_t(Token t, struct state s) {
  printf("%s\n", "compound_statement_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_BEGIN:
  case TOKEN_ID:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_optional_statements(t, s);
    t = match(TOKEN_END, t, s);
    break;
  case TOKEN_END:
    t = match(TOKEN_END, t, s);
    break;
  }
  return t;
}

Token parse_optional_statements(Token t, struct state s) {
  printf("%s\n", "optional_statements");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_BEGIN:
  case TOKEN_ID:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement_list(t, s);
    break;
  case TOKEN_END:
    break;
  }
  return t;
}

Token parse_statement_list(Token t, struct state s) {
  printf("%s\n", "statement_list");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_BEGIN:
  case TOKEN_ID:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement(t, s);
    t = parse_statement_list_t(t, s);
    break;
  }
  return t;
}

Token parse_statement_list_t(Token t, struct state s) {
  printf("%s\n", "statement_list_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_statement(t, s);
    t = parse_statement_list_t(t, s);
    break;
  case TOKEN_END:
    printf("%s\n", "statement_list_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_statement(Token t, struct state s) {
  printf("%s\n", "statement");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    break;
  case TOKEN_ID:
    t = parse_variable(t, s);
    t = match(TOKEN_ASSIGN, t, s);
    t = parse_expression(t, s);
    break;
  case TOKEN_IF:
    t = match(TOKEN_IF, t, s);
    t = parse_expression(t, s);
    t = match(TOKEN_THEN, t, s);
    t = parse_statement(t, s);
    t = parse_ifexp(t, s);
    break;
  case TOKEN_WHILE:
    t = match(TOKEN_WHILE, t, s);
    t = parse_expression(t, s);
    t = match(TOKEN_DO, t, s);
    t = parse_statement(t, s);
    break;
  }
  return t;
}

Token parse_ifexp(Token t, struct state s) {
  printf("%s\n", "ifexp");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_SEMICOLON:
    //case TOKEN_ELSE:
  case TOKEN_END:
    printf("%s\n", "ifexp_t ⟶ ε");
    break;
  case TOKEN_ELSE:
    t = match(TOKEN_ELSE, t, s);
    t = parse_statement(t, s);
    break;
  }
  return t;
}

Token parse_variable(Token t, struct state s) {
  printf("%s\n", "variable");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_variable_t(t, s);
    break;
  }
  return t;
}

Token parse_variable_t(Token t, struct state s) {
  printf("%s\n", "variable_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression(t, s);
    t = match(TOKEN_RPAREN, t, s);
    break;
  case TOKEN_ASSIGN:
    printf("%s\n", "variable_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_expression_list(Token t, struct state s) {
  printf("%s\n", "expression_list");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_LPAREN:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_NOT:
  case TOKEN_ADDOP:    
    t = parse_expression(t, s);
    t = parse_expression_list_t(t, s);
    break;
  }
  return t;
}

Token parse_expression_list_t(Token t, struct state s) {
  printf("%s\n", "expression_list_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = parse_expression(t, s);
    t = parse_expression_list_t(t, s);
    break;
  case TOKEN_RPAREN:
    printf("%s\n", "expression_list_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_expression(Token t, struct state s) {
  printf("%s\n", "expression");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_LPAREN:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_simple_expression(t, s);
    t = parse_expression_t(t, s);
    break;
  }
  return t;
}

Token parse_expression_t(Token t, struct state s) {
  printf("%s\n", "expression_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_RELOP:
    t = match(TOKEN_RELOP, t, s);
    t = parse_simple_expression(t, s);
    break;
  case TOKEN_RPAREN:
  case TOKEN_RBRACKET:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_COMMA:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_ELSE:
    printf("%s\n", "expression_t ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_simple_expression(Token t, struct state s) {
  printf("%s\n", "simple_expression");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_LPAREN:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_sexp(t, s);
    t = parse_sexp_t(t, s);
    break;
  }
  return t;
}

Token parse_sexp(Token t, struct state s) {
  printf("%s\n", "sexp");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_LPAREN:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_NOT:
    t = parse_term(t, s);
    break;
  case TOKEN_ADDOP:
    t = parse_sign(t, s);
    t = parse_term(t, s);
    break;
  }
  return t;
}

Token parse_sexp_t(Token t, struct state s) {
  printf("%s\n", "sexp_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_RELOP:
  case TOKEN_RPAREN:
  case TOKEN_RBRACKET:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_COMMA:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_ELSE:
    printf("%s\n", "sexp_t ⟶ ε");
    fflush(stdout);
    break;
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    t = parse_term(t, s);
    t = parse_sexp_t(t, s);
    break;
  }
  return t;
}

Token parse_term(Token t, struct state s) {
  printf("%s\n", "term");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_LPAREN:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_NOT:
    t = parse_factor(t, s);
    t = parse_term_t(t, s);
    break;
  }
  return t;
}

Token parse_term_t(Token t, struct state s) {
  printf("%s\n", "term_t");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ADDOP:
  case TOKEN_RELOP:
  case TOKEN_RBRACKET:
  case TOKEN_RPAREN:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_COMMA:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_ELSE:
    printf("%s\n", "term_t ⟶ ε");
    fflush(stdout);
    break;
  case TOKEN_MOD:
    t = match(TOKEN_MOD, t, s);
    t = parse_factor(t, s);
    t = parse_term_t(t, s);
    break;
  case TOKEN_MULOP:
    t = match(TOKEN_MULOP, t, s);
    t = parse_factor(t, s);
    t = parse_term_t(t, s);
    break;
  }
  return t;
}

Token parse_factor(Token t, struct state s) {
  printf("%s\n", "factor");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_fexp_list(t, s);
    break;
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression(t, s);
    t = match(TOKEN_RPAREN, t, s);
    break;
  case TOKEN_INT:
    t = match(TOKEN_INT, t, s);
    break;
  case TOKEN_REAL:
    t = match(TOKEN_REAL, t, s);
  case TOKEN_NOT:
    t = match(TOKEN_NOT, t, s);
    t = parse_factor(t, s);
    break;
  }
  return t;
}

Token parse_fexp_list(Token t, struct state s) {
  printf("%s\n", "fexp_list");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression_list(t, s);
    t = match(TOKEN_RPAREN, t, s);
    break;
  case TOKEN_MULOP:
  case TOKEN_MOD:
  case TOKEN_ADDOP:
  case TOKEN_RELOP:
  case TOKEN_RBRACKET:
  case TOKEN_RPAREN:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_COMMA:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_ELSE:
    printf("%s\n", "fexp_list ⟶ ε");
    fflush(stdout);
    break;
  }
  return t;
}

Token parse_sign(Token t, struct state s) {
  printf("%s\n", "sign");
  fflush(stdout);
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    break;
  }
  return t;
}
