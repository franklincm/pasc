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
static int print = 1;

void print_level(char * msg) {

  if(print) {
    for (int i = 0; i < level; i++) {
      printf("-");
    }
    printf("| ");
    printf("%s", msg);
  };
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

Token match(int token_type, Token t, struct state s, int *synch, int synch_size) {
  if(print) {
    for(int i = 0; i < level+1; i++) {
      printf(" ");
    }
    printf("# match: %s    %s\n", type_to_str(t.type), t.str);
    fflush(stdout);
  }
  if (t.type == token_type) {
    t = get_tok(s);
    if (token_type == TOKEN_EOF) {
      printf("END OF PARSE\n");
      fflush(stdout);
    } else {
      return t;
    }
  } else {
    printf("SYNTAX ERROR\n");
    fflush(stdout);
    printf("Expecting: %s, Got: %s    %s\n", type_to_str(token_type), type_to_str(t.type), t.str);
    fflush(stdout);

    // while not a synchronizing token, skip tokens
    while(t.type != token_type) {
      for (int token = 0; token < synch_size; token++) {
        if (synch[token] == t.type) {
          //printf("RESUME HERE\n");
          return t;
        }
      }
      t = get_tok(s);
    }
  }
  return t;
}

void parse(FILE *source,
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
  int synch[] = {
    TOKEN_EOF,
    TOKEN_PROGRAM
  };

  level++;
  // print_level();
  print_level("parse program\n");
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_LPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_identifier_list(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program\n");
    t = match(TOKEN_RPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_program_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program\n");
    t = parse_program_tail_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program\n");
    t = match(TOKEN_EOF, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_program_tail(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_FUNCTION, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_VAR, */
  /*   TOKEN_PROGRAM */
  /* }; */

  level++;
  // print_level();
  print_level("parse program_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program_tail\n");
    break;
  }
  return t;
}

Token parse_program_tail_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_PROGRAM
  };
  
  level++;
  // print_level();
  print_level("parse program_tail_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;

  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_identifier_list(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_ID,
    TOKEN_PROGRAM
  };

  level++;
  // print_level();
  print_level("parse identifier_list\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_identifier_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to identifier_list\n");
    break;
  }
  return t;
}

Token parse_identifier_list_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_ID
  };
  
  level++;
  // print_level();
  print_level("parse identifier_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_identifier_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to identifier_list_tail\n");
    break;
  }
  return t;
}

Token parse_declarations(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_VAR
  };

  level++;
  // print_level();
  print_level("parse declarations\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s, synch, sizeof(synch) / sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to declarations\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_declarations_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to declrations\n");
    break;
  }
  return t;
}

Token parse_declarations_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_VAR
  };
  
  level++;
  // print_level();
  print_level("parse declarations_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to declarations_tail\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_declarations_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to declarations_tail\n");
    break;
  }
  return t;
}

Token parse_type(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_INTEGER,
    TOKEN_RREAL,
    TOKEN_ARRAY,
    TOKEN_VAR
  };

  level++;
  // print_level();
  print_level("parse type\n");
  switch(t.type) {
  case TOKEN_INTEGER:
  case TOKEN_RREAL:
    t = parse_standard_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to type\n");
    break;

  case TOKEN_ARRAY:
    t = match(TOKEN_ARRAY, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_LBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_INT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ELIPSIS, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_INT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_RBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_OF, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_standard_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to type\n");
    break;
  }
  return t;
}

Token parse_standard_type(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_INTEGER,
    TOKEN_RREAL,
    TOKEN_LPAREN,
    TOKEN_COLON
  };

  level++;
  // print_level();
  print_level("parse standard_type\n");
  switch(t.type) {
  case TOKEN_INTEGER:
    t = match(TOKEN_INTEGER, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;

  case TOKEN_RREAL:
    t = match(TOKEN_RREAL, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_subprogram_declarations(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_BEGIN,
    TOKEN_FUNCTION
  };
  
  level++;
  // print_level();
  print_level("parse subprogram_declarations\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declarations\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declarations\n");
    break;
  }
  return t;
}

Token parse_subprogram_declarations_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_BEGIN,
    TOKEN_FUNCTION
  };
  
  level++;
  // print_level();
  print_level("parse subprogram_declarations_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declarations_tail\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declarations_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_SEMICOLON, */
  /*   TOKEN_FUNCTION */
  /* }; */

  level++;
  // print_level();
  print_level("parse subprogram_declaration\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_head(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration_tail(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_FUNCTION, */
  /*   TOKEN_VAR */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse subprogram_declaration_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_declaration_tail_tail(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_SEMICOLON, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_FUNCTION */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse subprogram_declaration_tail_tail\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    break;

  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    break;
  }
  return t;
}

Token parse_subprogram_head(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN
  };
  
  level++;
  // print_level();
  print_level("parse subprogram_head\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = match(TOKEN_FUNCTION, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_subprogram_head_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_head\n");
    break;
  }
  return t;
}

Token parse_subprogram_head_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_LPAREN,
    TOKEN_COLON
  };
  
  level++;
  // print_level();
  print_level("parse subprogram_head_tail\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = parse_arguments(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_standard_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;

  case TOKEN_COLON:
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_standard_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_arguments(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_COLON,
    TOKEN_LPAREN
  };
  
  level++;
  // print_level();
  print_level("parse argument\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_parameter_list(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to arguments\n");
    t = match(TOKEN_RPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_parameter_list(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_ID,
    TOKEN_LPAREN
  };
  
  level++;
  // print_level();
  print_level("parse parameter_list\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to parameter_list\n");
    t = parse_parameter_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to parameter_list\n");
    break;
  }
  return t;
}

Token parse_parameter_list_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_SEMICOLON,
    TOKEN_ID
  };
  
  level++;
  // print_level();
  print_level("parse parameter_list_tail\n");
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = match(TOKEN_COLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_type(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to parameter_list_tail\n");
    t = parse_parameter_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to parameter_list_tail\n");
    break;
  }
  return t;
}

Token parse_compound_statement(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_BEGIN,
    TOKEN_FUNCTION,
    TOKEN_ID,
    TOKEN_WHILE
  };
  
  level++;
  // print_level();
  print_level("parse compound_statement\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = match(TOKEN_BEGIN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_compound_statement_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to compound_statement\n");
    break;
  }
  return t;
}

Token parse_compound_statement_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_WHILE
  };
  
  level++;
  // print_level();
  print_level("parse compound_statement_tail\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_optional_statements(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to compound_statement_tail\n");
    t = match(TOKEN_END, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;

  case TOKEN_END:
    t = match(TOKEN_END, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_optional_statements(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_END, */
  /*   TOKEN_ID, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_IF, */
  /*   TOKEN_WHILE */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse optional_statements\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement_list(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to optional_statements\n");
    break;
  }
  return t;
}

Token parse_statement_list(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_END, */
  /*   TOKEN_ID, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_IF, */
  /*   TOKEN_WHILE */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse statement_list\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement_list\n");
    t = parse_statement_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement_list\n");
    break;
  }
  return t;
}

Token parse_statement_list_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_END,
    TOKEN_SEMICOLON,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_IF,
    TOKEN_WHILE
  };
  
  level++;
  // print_level();
  print_level("parse statement_list_tail\n");
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement_list_tail\n");
    t = parse_statement_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement_list_tail\n");
    break;
  }
  return t;
}

Token parse_statement(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_WHILE
  };

  level++;
  // print_level();
  print_level("parse statement\n");
  switch(t.type) {
  case TOKEN_ID:
    t = parse_variable(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    t = match(TOKEN_ASSIGN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    break;
    
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    break;
    
  case TOKEN_IF:
    t = parse_ifexp(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    break;
    
  case TOKEN_WHILE:
    t = match(TOKEN_WHILE, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    t = match(TOKEN_DO, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to statement\n");
    break;
  }
  return t;
}

Token parse_ifexp(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_WHILE
  };
  
  level++;
  // print_level();
  print_level("parse ifexp\n");
  switch(t.type) {
  case TOKEN_IF:
    t = match(TOKEN_IF, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to ifexp\n");
    t = match(TOKEN_THEN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to ifexp\n");
    t = parse_ifexp_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to ifexp\n");
    break;
  }
  return t;
}

Token parse_ifexp_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE
  };
  
  level++;
  // print_level();
  print_level("parse ifexp_tail\n");
  switch(t.type) {
  case TOKEN_ELSE:
    t = match(TOKEN_ELSE, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_statement(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to ifexp_tail\n");
    break;
  }
  return t;
}

Token parse_variable(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_ASSIGN,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_IF,
    TOKEN_WHILE
  };
  
  level++;
  // print_level();
  print_level("parse variable\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_variable_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to variable\n");
    break;
  }
  return t;
}

Token parse_variable_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_ASSIGN,
    TOKEN_LBRACKET,
    TOKEN_ID
  };
  
  level++;
  // print_level();
  print_level("parse variable_tail\n");
  switch(t.type) {
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to variable_tail\n");
    t = match(TOKEN_RBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_expression_list(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_RPAREN, */
  /*   TOKEN_ID, */
  /*   TOKEN_INT, */
  /*   TOKEN_REAL, */
  /*   TOKEN_NOT, */
  /*   TOKEN_LPAREN, */
  /*   TOKEN_ADDOP, */
  /*   TOKEN_LBRACKET */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse expression_list\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression_list\n");
    t = parse_expression_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression\n");
    break;
  }
  return t;
}

Token parse_expression_list_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_ADDOP
  };
  
  level++;
  // print_level();
  print_level("parse expression_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression_list_tail\n");
    t = parse_expression_list_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression_list_tail\n");
    break;
  }
  return t;
}

Token parse_expression(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_SEMICOLON, */
  /*   TOKEN_END, */
  /*   TOKEN_IF, */
  /*   TOKEN_ELSE, */
  /*   TOKEN_DO, */
  /*   TOKEN_THEN, */
  /*   TOKEN_ID, */
  /*   TOKEN_INT, */
  /*   TOKEN_REAL, */
  /*   TOKEN_LPAREN, */
  /*   TOKEN_NOT, */
  /*   TOKEN_ADDOP, */
  /*   TOKEN_BEGIN, */
  /*   TOKEN_WHILE, */
  /*   TOKEN_LBRACKET */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse expression\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
  case TOKEN_ADDOP:
    t = parse_simple_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression\n");
    t = parse_expression_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression\n");
    break;
  }
  return t;
}

Token parse_expression_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_THEN,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_RPAREN,
    TOKEN_RELOP,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_LPAREN,
    TOKEN_NOT,
    TOKEN_ADDOP
  };
  
  level++;
  // print_level();
  print_level("parse expression_tail\n");
  switch(t.type) {
  case TOKEN_RELOP:
    t = match(TOKEN_RELOP, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_simple_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression_tail\n");
    t = parse_expression_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to expression_tail\n");
    break;
  }
  return t;
}

Token parse_simple_expression(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_RELOP, */
  /*   TOKEN_SEMICOLON, */
  /*   TOKEN_END, */
  /*   TOKEN_IF, */
  /*   TOKEN_ELSE, */
  /*   TOKEN_DO, */
  /*   TOKEN_THEN, */
  /*   TOKEN_RBRACKET, */
  /*   TOKEN_COMMA, */
  /*   TOKEN_LPAREN, */
  /*   TOKEN_ID, */
  /*   TOKEN_INT, */
  /*   TOKEN_REAL, */
  /*   TOKEN_LPAREN, */
  /*   TOKEN_NOT, */
  /*   TOKEN_ADDOP */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse simple_expression\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_term(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression\n");
    break;
    
  case TOKEN_ADDOP:
    t = parse_sign(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression\n");
    t = parse_term(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression\n");
    break;
  }
  return t;
}

Token parse_simple_expression_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RELOP,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_THEN,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_RPAREN,
    TOKEN_ADDOP,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT,
    TOKEN_LPAREN
  };
  
  level++;
  // print_level();
  print_level("parse simple_expression_tail\n");
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_term(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression_tail\n");
    t = parse_simple_expression_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to simple_expression_tail\n");
    break;
  }
  return t;
}

Token parse_term(Token t, struct state s) {
  /* int synch[] = { */
  /*   TOKEN_EOF, */
  /*   TOKEN_ADDOP, */
  /*   TOKEN_ID, */
  /*   TOKEN_INT, */
  /*   TOKEN_REAL, */
  /*   TOKEN_NOT, */
  /*   TOKEN_LPAREN */
  /* }; */
  
  level++;
  // print_level();
  print_level("parse term\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term\n");
    t = parse_term_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term\n");
    break;
  }
  return t;
}

Token parse_term_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_ADDOP,
    TOKEN_MULOP,
    TOKEN_ID,
    TOKEN_NOT,
    TOKEN_LPAREN
  };
  
  level++;
  // print_level();
  print_level("parse term_tail\n");
  switch(t.type) {
  case TOKEN_MULOP:
    t = match(TOKEN_MULOP, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_MOD:
    t = match(TOKEN_MOD, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_DIV:
    t = match(TOKEN_DIV, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_AND:
    t = match(TOKEN_AND, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    t = parse_term_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to term_tail\n");
    break;
  }
  return t;
}

Token parse_factor(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_MULOP,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT,
    TOKEN_LPAREN
  };
  
  level++;
  // print_level();
  print_level("parse factor\n");
  switch(t.type) {
  case TOKEN_ID:
    t = match(TOKEN_ID, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor_tail(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to factor\n");
    break;
    
  case TOKEN_INT:
    t = match(TOKEN_INT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
    
  case TOKEN_REAL:
    t = match(TOKEN_REAL, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
    
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to factor\n");
    t = match(TOKEN_RPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
    
  case TOKEN_NOT:
    t = match(TOKEN_NOT, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_factor(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to factor\n");
    break;
  }
  return t;
}

Token parse_factor_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_MULOP,
    TOKEN_LBRACKET,
    TOKEN_LPAREN,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT
  };
  
  level++;
  // print_level();
  print_level("parse factor_tail\n");
  switch(t.type) {
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to factor_tail\n");
    t = match(TOKEN_RBRACKET, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;

  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    t = parse_expression_list(t, s);
    level--;
    // print_level();
    print_level("*RETURN* to factor_tail\n");
    t = match(TOKEN_RPAREN, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}

Token parse_sign(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_LPAREN,
    TOKEN_NOT,
    TOKEN_ADDOP
  };
  
  level++;
  // print_level();
  print_level("parse sign\n");
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s, synch, sizeof(synch)/sizeof(synch[0]));
    break;
  }
  return t;
}
