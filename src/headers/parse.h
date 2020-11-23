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
Token synchronize(Token t, struct state s, int *synch, int size, char *production);
void parse(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           node reserved_words,
           node *symbol_table);
Token parse_program(Token t, struct state s);
Token parse_program_tail(Token t, struct state s);
Token parse_program_tail_tail(Token t, struct state s);
Token parse_identifier_list(Token t, struct state s);
Token parse_identifier_list_tail(Token t, struct state s);
Token parse_declarations(Token t, struct state s);
Token parse_declarations_tail(Token t, struct state s);
Token parse_type(Token t, struct state s);
Token parse_standard_type(Token t, struct state s);
Token parse_subprogram_declarations(Token t, struct state s);
Token parse_subprogram_declarations_tail(Token t, struct state s);
Token parse_subprogram_declaration(Token t, struct state s);
Token parse_subprogram_declaration_tail(Token t, struct state s);
Token parse_subprogram_declaration_tail_tail(Token t, struct state s);
Token parse_subprogram_head(Token t, struct state s);
Token parse_subprogram_head_tail(Token t, struct state s);
Token parse_arguments(Token t, struct state s);
Token parse_parameter_list(Token t, struct state s);
Token parse_parameter_list_tail(Token t, struct state s);
Token parse_compound_statement(Token t, struct state s);
Token parse_compound_statement_tail(Token t, struct state s);
Token parse_optional_statements(Token t, struct state s);
Token parse_statement_list(Token t, struct state s);
Token parse_statement_list_tail(Token t, struct state s);
Token parse_statement(Token t, struct state s);
Token parse_ifexp(Token t, struct state s);
Token parse_ifexp_tail(Token t, struct state s);
Token parse_variable(Token t, struct state s);
Token parse_variable_tail(Token t, struct state s);
Token parse_expression_list(Token t, struct state s);
Token parse_expression_list_tail(Token t, struct state s);
Token parse_expression(Token t, struct state s);
Token parse_expression_tail(Token t, struct state s);
Token parse_simple_expression(Token t, struct state s);
Token parse_simple_expression_tail(Token t, struct state s);
Token parse_term(Token t, struct state s);
Token parse_term_tail(Token t, struct state s);
Token parse_factor(Token t, struct state s);
Token parse_factor_tail(Token t, struct state s);
Token parse_sign(Token t, struct state s);
