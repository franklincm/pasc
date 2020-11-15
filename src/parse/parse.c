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

static int level = 0;

void print_level() {
  for (int i = 0; i < level; i++) {
    printf("-");
  }
  printf("| ");
}

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
  for(int i = 0; i < level+1; i++) {
    printf(" ");
  }
  printf("# match: %s    %s\n", type_to_str(t.type), t.str);
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
  level++;
  print_level();
  printf("parse program\n");
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_LPAREN, t, s);
    t = parse_identifier_list(t, s);
    level--;
    print_level();
    printf("*RETURN* to program\n");
    t = match(TOKEN_RPAREN, t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_program_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to program\n");
    t = parse_program_tail_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to program\n");
    t = match(TOKEN_EOF, t, s);
    break;
  }
  return t;
}

Token parse_program_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse program_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    print_level();
    printf("*RETURN* to program_tail\n");
    break;
  }
  return t;
}

Token parse_program_tail_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse program_tail_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    print_level();
    printf("*RETURN* to program_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s);
    break;

  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s);
    break;

  
  }
  return t;
}

Token parse_identifier_list(Token t, struct state s) {
  level++;
  print_level();
  printf("parse identifier_list\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to identifier_list\n");
    break;
  }
  return t;
}

Token parse_identifier_list_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse identifier_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to identifier_list_tail\n");
    break;
  }
  return t;
}

Token parse_declarations(Token t, struct state s) {
  level++;
  print_level();
  printf("parse declarations\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to declarations\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to declrations\n");
    break;
  }
  return t;
}

Token parse_declarations_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse declarations_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to declarations_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to declarations_tail\n");
    break;
  }
  return t;
}

Token parse_type(Token t, struct state s) {
  level++;
  print_level();
  printf("parse type\n");
  switch(t.type) {
  case TOKEN_INTEGER:
  case TOKEN_RREAL:
    t = parse_standard_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to type\n");
    break;

  case TOKEN_ARRAY:
    t = match(TOKEN_ARRAY, t, s);
    t = match(TOKEN_LBRACKET, t, s);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_ELIPSIS, t, s);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_RBRACKET, t, s);
    t = match(TOKEN_OF, t, s);
    t = parse_standard_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to type\n");
    break;
  }
  return t;
}

Token parse_standard_type(Token t, struct state s) {
  level++;
  print_level();
  printf("parse standard_type\n");
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
  level++;
  print_level();
  printf("parse subprogram_declarations\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declarations\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declarations\n");
    break;
  }
  return t;
}

Token parse_subprogram_declarations_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_declarations_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declarations_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declarations_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_declaration\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_head(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_declaration_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration_tail_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_declaration_tail_tail\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration_tail_tail\n");
    break;

  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_declaration_tail_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_head(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_head\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = match(TOKEN_FUNCTION, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_subprogram_head_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_head\n");
    break;
  }
  return t;
}

Token parse_subprogram_head_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse subprogram_head_tail\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = parse_arguments(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    break;

  case TOKEN_COLON:
    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    break;
  }
  return t;
}

Token parse_arguments(Token t, struct state s) {
  level++;
  print_level();
  printf("parse argument\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_parameter_list(t, s);
    level--;
    print_level();
    printf("*RETURN* to arguments\n");
    t = match(TOKEN_RPAREN, t, s);
    break;
  }
  return t;
}

Token parse_parameter_list(Token t, struct state s) {
  level++;
  print_level();
  printf("parse parameter_list\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to parameter_list\n");
    t = parse_parameter_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to parameter_list\n");
    break;
  }
  return t;
}

Token parse_parameter_list_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse parameter_list_tail\n");
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level();
    printf("*RETURN* to parameter_list_tail\n");
    t = parse_parameter_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to parameter_list_tail\n");
    break;
  }
  return t;
}

Token parse_compound_statement(Token t, struct state s) {
  level++;
  print_level();
  printf("parse compound_statement\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = match(TOKEN_BEGIN, t, s);
    t = parse_compound_statement_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to compound_statement\n");
    break;
  }
  return t;
}

Token parse_compound_statement_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse compound_statement_tail\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_optional_statements(t, s);
    level--;
    print_level();
    printf("*RETURN* to compound_statement_tail\n");
    t = match(TOKEN_END, t, s);
    break;

  case TOKEN_END:
    t = match(TOKEN_END, t, s);
    break;
  }
  return t;
}

Token parse_optional_statements(Token t, struct state s) {
  level++;
  print_level();
  printf("parse optional_statements\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement_list(t, s);
    level--;
    print_level();
    printf("*RETURN* to optional_statements\n");
    break;
  }
  return t;
}

Token parse_statement_list(Token t, struct state s) {
  level++;
  print_level();
  printf("parse statement_list\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement_list\n");
    t = parse_statement_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement_list\n");
    break;
  }
  return t;
}

Token parse_statement_list_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse statement_list_tail\n");
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement_list_tail\n");
    t = parse_statement_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement_list_tail\n");
    break;
  }
  return t;
}

Token parse_statement(Token t, struct state s) {
  level++;
  print_level();
  printf("parse statement\n");
  switch(t.type) {
  case TOKEN_ID:
    t = parse_variable(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    t = match(TOKEN_ASSIGN, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    break;
    
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    break;
    
  case TOKEN_IF:
    t = parse_ifexp(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    break;
    
  case TOKEN_WHILE:
    t = match(TOKEN_WHILE, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    t = match(TOKEN_DO, t, s);
    t = parse_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to statement\n");
    break;
  }
  return t;
}

Token parse_ifexp(Token t, struct state s) {
  level++;
  print_level();
  printf("parse ifexp\n");
  switch(t.type) {
  case TOKEN_IF:
    t = match(TOKEN_IF, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to ifexp\n");
    t = match(TOKEN_THEN, t, s);
    t = parse_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to ifexp\n");
    t = parse_ifexp_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to ifexp\n");
    break;
  }
  return t;
}

Token parse_ifexp_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse ifexp_tail\n");
  switch(t.type) {
  case TOKEN_ELSE:
    t = match(TOKEN_ELSE, t, s);
    t = parse_statement(t, s);
    level--;
    print_level();
    printf("*RETURN* to ifexp_tail\n");
    break;
  }
  return t;
}

Token parse_variable(Token t, struct state s) {
  level++;
  print_level();
  printf("parse variable\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_variable_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to variable\n");
    break;
  }
  return t;
}

Token parse_variable_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse variable_tail\n");
  switch(t.type) {
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to variable_tail\n");
    t = match(TOKEN_RBRACKET, t, s);
    break;
  }
  return t;
}

Token parse_expression_list(Token t, struct state s) {
  level++;
  print_level();
  printf("parse expression_list\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression_list\n");
    t = parse_expression_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression\n");
    break;
  }
  return t;
}

Token parse_expression_list_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse expression_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression_list_tail\n");
    t = parse_expression_list_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression_list_tail\n");
    break;
  }
  return t;
}

Token parse_expression(Token t, struct state s) {
  level++;
  print_level();
  printf("parse expression\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_simple_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression\n");
    t = parse_expression_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression\n");
    break;
  }
  return t;
}

Token parse_expression_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse expression_tail\n");
  switch(t.type) {
  case TOKEN_RELOP:
    t = match(TOKEN_RELOP, t, s);
    t = parse_simple_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression_tail\n");
    t = parse_expression_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to expression_tail\n");
    break;
  }
  return t;
}

Token parse_simple_expression(Token t, struct state s) {
  level++;
  print_level();
  printf("parse simple_expression\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_term(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression\n");
    break;
    
  case TOKEN_ADDOP:
    t = parse_sign(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression\n");
    t = parse_term(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression\n");
    break;
  }
  return t;
}

Token parse_simple_expression_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse simple_expression_tail\n");
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    t = parse_term(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression_tail\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to simple_expression_tail\n");
    break;
  }
  return t;
}

Token parse_term(Token t, struct state s) {
  level++;
  print_level();
  printf("parse term\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to term\n");
    t = parse_term_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to term\n");
    break;
  }
  return t;
}

Token parse_term_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse term_tail\n");
  switch(t.type) {
  case TOKEN_MULOP:
    t = match(TOKEN_MULOP, t, s);
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    break;

  case TOKEN_MOD:
    t = match(TOKEN_MOD, t, s);
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    break;

  case TOKEN_DIV:
    t = match(TOKEN_DIV, t, s);
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    break;

  case TOKEN_AND:
    t = match(TOKEN_AND, t, s);
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to term_tail\n");
    break;
  }
  return t;
}

Token parse_factor(Token t, struct state s) {
  level++;
  print_level();
  printf("parse factor\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s);
    t = parse_factor_tail(t, s);
    level--;
    print_level();
    printf("*RETURN* to factor\n");
    break;
    
  case TOKEN_INT:
    t = match(TOKEN_INT, t, s);
    break;
    
  case TOKEN_REAL:
    t = match(TOKEN_REAL, t, s);
    break;
    
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to factor\n");
    t = match(TOKEN_RPAREN, t, s);
    break;
    
  case TOKEN_NOT:
    t = match(TOKEN_NOT, t, s);
    t = parse_factor(t, s);
    level--;
    print_level();
    printf("*RETURN* to factor\n");
    break;
  }
  return t;
}

Token parse_factor_tail(Token t, struct state s) {
  level++;
  print_level();
  printf("parse factor_tail\n");
  switch(t.type) {
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s);
    t = parse_expression(t, s);
    level--;
    print_level();
    printf("*RETURN* to factor_tail\n");
    t = match(TOKEN_RBRACKET, t, s);
    break;

  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression_list(t, s);
    level--;
    print_level();
    printf("*RETURN* to factor_tail\n");
    t = match(TOKEN_RPAREN, t, s);
    break;
  }
  return t;
}

Token parse_sign(Token t, struct state s) {
  level++;
  print_level();
  printf("parse sign\n");
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    break;
  }
  return t;
}
