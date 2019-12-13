#ifndef TOKEN
#define TOKEN
#include "token.h"
#endif

struct state
{
  FILE *source;
  FILE *listing;
  FILE *tokenfile;
  node reserved_words;
  node *symbol_table;
};

Token get_tok(struct state s);
Token match(int token_type, Token t, struct state s);

void parse(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           node reserved_words,
           node *symbol_table);

void parse_test(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           node reserved_words,
           node *symbol_table);

Token parse_program(Token t, struct state s);
Token parse_program_start(Token t, struct state s);
Token parse_program_declarations(Token t, struct state s);
Token parse_program_subprogram_declarations(Token t, struct state s);
Token parse_identifier_list(Token t, struct state s);
Token parse_identifier_list_t(Token t, struct state s);
Token parse_declarations(Token t, struct state s);
Token parse_declarations_t(Token t, struct state s);
Token parse_type(Token t, struct state s);
Token parse_standard_type(Token t, struct state s);
Token parse_subprogram_declarations(Token t, struct state s);
Token parse_subprogram_declarations_t(Token t, struct state s);
Token parse_subprogram_declaration(Token t, struct state s);
Token parse_subprogram_head(Token t, struct state s);
Token parse_subprogram_head_t(Token t, struct state s);
Token parse_arguments(Token t, struct state s);
Token parse_parameter_list(Token t, struct state s);
Token parse_parameter_list_t(Token t, struct state s);
Token parse_compound_statement(Token t, struct state s);
Token parse_compound_statement_t(Token t, struct state s);
Token parse_optional_statements(Token t, struct state s);
Token parse_statement_list(Token t, struct state s);
Token parse_statement_list_t(Token t, struct state s);
Token parse_statement(Token t, struct state s);
Token parse_ifexp(Token t, struct state s);
Token parse_variable(Token t, struct state s);
Token parse_variable_t(Token t, struct state s);
Token parse_expression_list(Token t, struct state s);
Token parse_expression_list_t(Token t, struct state s);
Token parse_expression(Token t, struct state s);
Token parse_expression_t(Token t, struct state s);
Token parse_simple_expression(Token t, struct state s);
Token parse_sexp(Token t, struct state s);
Token parse_sexp_t(Token t, struct state s);
Token parse_term(Token t, struct state s);
Token parse_term_t(Token t, struct state s);
Token parse_factor(Token t, struct state s);
Token parse_fexp_list(Token t, struct state s);
Token parse_sign(Token t, struct state s);
