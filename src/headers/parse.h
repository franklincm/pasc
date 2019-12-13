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
Token parse_compound_statement(Token t, struct state s);
Token parse_identifier_list(Token t, struct state s);
Token parse_identifier_list_t(Token t, struct state s);
Token parse_declarations(Token t, struct state s);
Token parse_declarations_t(Token t, struct state s);
Token parse_type(Token t, struct state s);
Token parse_standard_type(Token t, struct state s);
