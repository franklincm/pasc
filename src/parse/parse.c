#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifndef COLOR
#define COLOR
#include "../headers/colornode.h"
#endif

#ifndef STACK
#define STACK
#include "../headers/stack.h"
#endif

int global_offset;
int local_offset;
int width;

node *sym_table;

static int level = 0;
static int print = 0;
static int EOP = 0;


char *param_id_str;
char *dec_id_str;
char *lhs;
char *profile_buffer;

int type;
int standard_type;
int statement_compound;

char buffer [200];

int semerr_line = 0;
int synerr_line = 0;

/* TODO: ^L-Attribute Definition */

int get_semerr_line() {
  return semerr_line;
}

int get_synerr_line() {
  return synerr_line;
}

void print_semerr(char *msg, FILE *listing) {
  char *sbuffer;
  char *prefix = "SEMERR: ";
  sbuffer = malloc(sizeof(char) * (strlen(msg) + strlen(prefix)));
  sprintf(sbuffer, "%s%s", prefix, msg);

  if (get_lineno() != semerr_line
      && get_lineno() != synerr_line) {
    write_line_to_file(sbuffer, listing);
    semerr_line = get_lineno();
  }
}

void print_level(char *msg) {

  if(print) {
    for (int i = 0; i < level; i++) {
      printf("-");
    }
    printf("| ");
    printf("%s", msg);
  };
}

Token get_tok(struct state s) {
  Token t;
  if (!EOP) {
    t = get_token(s.source,
                  s.listing,
                  s.tokenfile,
                  s.reserved_words);
    while(t.type == TOKEN_WS) {
      t = get_token(s.source,
                    s.listing,
                    s.tokenfile,
                    s.reserved_words);

    }
    if (t.type == TOKEN_EOF) {
      EOP = 1;
    }
    return t;
  }
  t.str = "UNRECSYM";
  t.type = TOKEN_UNRECOGNIZED_SYMBOL;
  return t;

}

Token match(int token_type, Token t, struct state s) {
  if(t.type == TOKEN_EOF) {
    write_line_to_file("End of Parse\n", s.listing);
    //exit(0);
    return t;
  }
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
      return t;
    } else {
      return t;
    }
  } else {
    printf("SYNERR\n");
    fflush(stdout);
    printf("Expecting: %s, Got: %s    %s\n", type_to_str(token_type), type_to_str(t.type), t.str);
    fflush(stdout);
    write_line_to_file("SYNERR\n", s.listing);


    char * buffer = (char *)malloc(150 * sizeof(char));
    //sprintf(buffer, "Missing: '%s'\n", type_to_str(token_type));
    sprintf(buffer, "Expecting: %s, Got: '%s'\n", type_to_str(token_type), t.str);
    write_line_to_file(buffer, s.listing);
    synerr_line = get_lineno();

    // syntax error, skip token
    t = get_tok(s);    
  }
  return t;
}

Token synchronize(Token t, struct state s, int *synch, int size, char *production) {
  char * buffer = (char *)malloc(150 * sizeof(char));
  sprintf(buffer, "SYNERR\nExpecting %s, Got: '%s'\n", production, t.str);
  synerr_line = get_lineno();
  //write_line_to_file(buffer, s.listing);
  //write_line_to_file("synching...\n\n", s.listing);

  /* sprintf(buffer, "Expecting %s, Got: %s", production, t.str); */
  /* write_line_to_file(listing_err(buffer), s.listing); */

  if(t.type == TOKEN_EOF) {
    write_line_to_file("End of Parse\n", s.listing);
    //exit(0);
  }
  
  printf("SYNCH CALLED (%s): %s\n", production, t.str);

  for(int token = 0; token < size; token++) {
    if (t.type == synch[token]) {
      sprintf(buffer, "skipping %s...\n\n", t.str);
      write_line_to_file(buffer, s.listing);
      printf("SYNCH FOUND: %s\n\n", t.str);
      return t;
    }
  }
  
  int synch_found = 0;

  while (!synch_found) {

    for(int token = 0; token < size; token++) {

      if (t.type == synch[token]) {
        synch_found = 1;
        sprintf(buffer, "skipping...\n\n");
        //write_line_to_file(buffer, s.listing);
        printf("SYNCH FOUND: %s\n\n", t.str);
        return t;
      }
    }
    t = get_tok(s);
  }
}

void parse(FILE *source,
           FILE *listing,
           FILE *tokenfile,
           FILE *symboltablefile,
           node reserved_words) {

  //sym_table = symbol_table;
  
  struct state s = {source, listing, tokenfile, symboltablefile, reserved_words};
  
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
  
  print_level("parse program\n");
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s);

    if(t.type != LEXERR)
      check_add_green(t.str, PGNAME, "", s.symboltablefile);
    
    t = match(TOKEN_ID, t, s);
    
    t = match(TOKEN_LPAREN, t, s);
    t = parse_identifier_list(t, s);
    level--;
    
    print_level("*RETURN* to program\n");
    
    t = match(TOKEN_RPAREN, t, s);
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_program_tail(t, s);
    level--;
    
    print_level("*RETURN* to program\n");
    t = parse_program_tail_tail(t, s);
    level--;
    
    print_level("*RETURN* to program\n");
    t = match(TOKEN_EOF, t, s);
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "program");
  }
  return t;
}

Token parse_program_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_VAR,
    TOKEN_PROGRAM
  };

  level++;
  
  print_level("parse program_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    
    print_level("*RETURN* to program_tail\n");
    break;
  case TOKEN_FUNCTION:
  case TOKEN_BEGIN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "program_tail");
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
  
  print_level("parse program_tail_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    
    print_level("*RETURN* to program_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    
    print_level("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s);
    break;

  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    
    print_level("*RETURN* to program_tail_tail\n");
    t = match(TOKEN_DOT, t, s);
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "program_tail_tail");
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
  print_level("parse identifier_list\n");

  switch(t.type) {
  case TOKEN_ID:
    if(t.type != LEXERR)
      check_add_blue(t.str, PGPARAM, s.symboltablefile);
    
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_tail(t, s);
    level--;
    print_level("*RETURN* to identifier_list\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "identifier list"); 
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
  
  print_level("parse identifier_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);

    if(t.type != LEXERR)
      check_add_blue(t.str, PGPARAM, s.symboltablefile);
    
    t = match(TOKEN_ID, t, s);
    t = parse_identifier_list_tail(t, s);
    level--;
    print_level("*RETURN* to identifier_list_tail\n");
    break;
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "identifier list tail");
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
  print_level("parse declarations\n");

  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);

    if (t.type != LEXERR)
      dec_id_str = t.str;
    
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level("*RETURN* to declarations\n");

    if(dec_id_str)
      check_add_blue(dec_id_str, type, s.symboltablefile);
    dec_id_str = NULL;
    
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_tail(t, s);
    level--;
    print_level("*RETURN* to declrations\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "declarations");
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
  print_level("parse declarations_tail\n");
  
  switch(t.type) {
  case TOKEN_VAR:
    t = match(TOKEN_VAR, t, s);

    if (t.type != LEXERR)
      dec_id_str = t.str;

    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level("*RETURN* to declarations_tail\n");

    if(dec_id_str)
      check_add_blue(dec_id_str, type, s.symboltablefile);
    dec_id_str = NULL;
    
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_declarations_tail(t, s);
    level--;
    print_level("*RETURN* to declarations_tail\n");
    break;
  case TOKEN_FUNCTION:
  case TOKEN_BEGIN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "declarations tail");
  }
  return t;
}

Token parse_type(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_FUNCTION,
    TOKEN_BEGIN,
    TOKEN_INTEGER,
    TOKEN_RREAL,
    TOKEN_ARRAY,
    TOKEN_VAR
  };

  level++;
  print_level("parse type\n");

  switch(t.type) {
  case TOKEN_INTEGER:
  case TOKEN_RREAL:
    t = parse_standard_type(t, s);
    level--;
    print_level("*RETURN* to type\n");

    type = standard_type;
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
    print_level("*RETURN* to type\n");

    type = standard_type + 4;
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "type");
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
  
  print_level("parse standard_type\n");
  switch(t.type) {
  case TOKEN_INTEGER:
    t = match(TOKEN_INTEGER, t, s);
    standard_type = t_INT;
    break;

  case TOKEN_RREAL:
    t = match(TOKEN_RREAL, t, s);
    standard_type = t_REAL;
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "standard type");
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
  
  print_level("parse subprogram_declarations\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declarations\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declarations\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram declarations");
  }
  return t;
}

Token parse_subprogram_declarations_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_FUNCTION,
    TOKEN_BEGIN
  };
  
  level++;
  
  print_level("parse subprogram_declarations_tail\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_declaration(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declarations_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_subprogram_declarations_tail(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declarations_tail\n");
    break;
  case TOKEN_BEGIN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram declarations tail");
  }
  return t;
}

Token parse_subprogram_declaration(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_FUNCTION
  };

  level++;
  
  print_level("parse subprogram_declaration\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = parse_subprogram_head(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration\n");
    t = parse_subprogram_declaration_tail_tail(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration_tail\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram declaration");
  }
  return t;
}

Token parse_subprogram_declaration_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_BEGIN,
    TOKEN_FUNCTION,
    TOKEN_VAR
  };
  
  level++;
  
  print_level("parse subprogram_declaration_tail\n");
  switch(t.type) {
  case TOKEN_VAR:
    t = parse_declarations(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration_tail\n");
    break;
  case TOKEN_FUNCTION:
  case TOKEN_BEGIN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram_declaration_tail");
  }
  return t;
}

Token parse_subprogram_declaration_tail_tail(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_BEGIN,
    TOKEN_FUNCTION
  };
  
  level++;
  
  print_level("parse subprogram_declaration_tail_tail\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    break;

  case TOKEN_FUNCTION:
    t = parse_subprogram_declarations(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    t = parse_compound_statement(t, s);
    level--;
    
    print_level("*RETURN* to subprogram_declaration_tail_tail\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram declaration tail tail");
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
  
  print_level("parse subprogram_head\n");
  switch(t.type) {
  case TOKEN_FUNCTION:
    t = match(TOKEN_FUNCTION, t, s);
    t = match(TOKEN_ID, t, s);
    t = parse_subprogram_head_tail(t, s);
    level--;
    print_level("*RETURN* to subprogram_head\n");
    
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram head");
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
  
  print_level("parse subprogram_head_tail\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = parse_arguments(t, s);
    level--;
    print_level("*RETURN* to subprogram_head_tail\n");

    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    level--;
    print_level("*RETURN* to subprogram_head_tail\n");

    t = match(TOKEN_SEMICOLON, t, s);
    break;

  case TOKEN_COLON:
    t = match(TOKEN_COLON, t, s);

    t = parse_standard_type(t, s);
    level--;

    print_level("*RETURN* to subprogram_head_tail\n");
    t = match(TOKEN_SEMICOLON, t, s);
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "subprogram head tail");
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
  
  print_level("parse argument\n");
  switch(t.type) {
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_parameter_list(t, s);
    level--;
    
    print_level("*RETURN* to arguments\n");
    t = match(TOKEN_RPAREN, t, s);
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "arguments");
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
  print_level("parse parameter_list\n");
  
  switch(t.type) {
  case TOKEN_ID:
    if(t.type != LEXERR)
      param_id_str = t.str;

    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);

    if(param_id_str)
      check_add_blue(param_id_str, type, s.symboltablefile);
    param_id_str = NULL;
    
    level--;
    print_level("*RETURN* to parameter_list\n");
    
    t = parse_parameter_list_tail(t, s);
    level--;
    
    print_level("*RETURN* to parameter_list\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "parameter list");
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
  print_level("parse parameter_list_tail\n");
  
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);

    if(t.type != LEXERR)
      param_id_str = t.str;
    
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);

    if(param_id_str)
      check_add_blue(param_id_str, type, s.symboltablefile);
    param_id_str = NULL;

    level--;
    print_level("*RETURN* to parameter_list_tail\n");
    
    t = parse_parameter_list_tail(t, s);
    level--;
    
    print_level("*RETURN* to parameter_list_tail\n");
    break;
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "parameter list tail");
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
  
  print_level("parse compound_statement\n");
  switch(t.type) {
  case TOKEN_BEGIN:
    t = match(TOKEN_BEGIN, t, s);
    t = parse_compound_statement_tail(t, s);
    level--;
    
    print_level("*RETURN* to compound_statement\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "compound statement");
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
  
  print_level("parse compound_statement_tail\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_optional_statements(t, s);
    level--;
    
    print_level("*RETURN* to compound_statement_tail\n");
    t = match(TOKEN_END, t, s);
    if (!statement_compound)
      pop_eye();
    break;

  case TOKEN_END:
    t = match(TOKEN_END, t, s);
    if (!statement_compound)
      pop_eye();
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "compound statement tail");
  }
  return t;
}

Token parse_optional_statements(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_END,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_IF,
    TOKEN_WHILE
  };
  
  level++;
  
  print_level("parse optional_statements\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement_list(t, s);
    level--;
    
    print_level("*RETURN* to optional_statements\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "optional statements");
  }
  return t;
}

Token parse_statement_list(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_END,
    TOKEN_ID,
    TOKEN_BEGIN,
    TOKEN_IF,
    TOKEN_WHILE
  };
  
  level++;
  
  print_level("parse statement_list\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_BEGIN:
  case TOKEN_IF:
  case TOKEN_WHILE:
    t = parse_statement(t, s);
    level--;
    
    print_level("*RETURN* to statement_list\n");
    t = parse_statement_list_tail(t, s);
    level--;
    
    print_level("*RETURN* to statement_list\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "statement list");
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
  
  print_level("parse statement_list_tail\n");
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);
    t = parse_statement(t, s);
    level--;
    
    print_level("*RETURN* to statement_list_tail\n");
    t = parse_statement_list_tail(t, s);
    level--;
    
    print_level("*RETURN* to statement_list_tail\n");
    break;
  case TOKEN_END:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "statement list tail");
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
  
  print_level("parse statement\n");
  switch(t.type) {
  case TOKEN_ID:

    t = parse_variable(t, s);
    level--;
    print_level("*RETURN* to statement\n");

    t = match(TOKEN_ASSIGN, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to statement\n");

    break;
    
  case TOKEN_BEGIN:
    t = parse_compound_statement(t, s);
    level--;
    print_level("*RETURN* to statement\n");
    break;
    
  case TOKEN_IF:
    t = parse_ifexp(t, s);
    level--;
    print_level("*RETURN* to statement\n");
    break;
    
  case TOKEN_WHILE:
    t = match(TOKEN_WHILE, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to statement\n");

    t = match(TOKEN_DO, t, s);
    t = parse_statement(t, s);

    level--;
    print_level("*RETURN* to statement\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "statement");
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
  
  print_level("parse ifexp\n");
  switch(t.type) {
  case TOKEN_IF:
    t = match(TOKEN_IF, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to ifexp\n");

    t = match(TOKEN_THEN, t, s);
    t = parse_statement(t, s);
    level--;
    print_level("*RETURN* to ifexp\n");

    t = parse_ifexp_tail(t, s);
    level--;
    print_level("*RETURN* to ifexp\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "if expression");
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
  
  print_level("parse ifexp_tail\n");
  switch(t.type) {
  case TOKEN_ELSE:
    t = match(TOKEN_ELSE, t, s);
    t = parse_statement(t, s);
    level--;
    
    print_level("*RETURN* to ifexp_tail\n");
    break;
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_IF:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "if expression tail");
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
  
  print_level("parse variable\n");
  switch(t.type) {
  case TOKEN_ID:

    t = match(TOKEN_ID, t, s);
    t = parse_variable_tail(t, s);
    level--;
    print_level("*RETURN* to variable\n");

    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "variable");
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
  
  print_level("parse variable_tail\n");
  switch(t.type) {
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to variable_tail\n");

    
    t = match(TOKEN_RBRACKET, t, s);
    break;
  case TOKEN_ASSIGN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "variable tail");
  }
  return t;
}

Token parse_expression_list(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_RPAREN,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_ADDOP,
    TOKEN_LBRACKET
  };
  
  level++;
  
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
    print_level("*RETURN* to expression_list\n");

    t = parse_expression_list_tail(t, s);
    level--;
    print_level("*RETURN* to expression_list\n");

    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "expression list");
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
  
  print_level("parse expression_list_tail\n");
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to expression_list_tail\n");

    t = parse_expression_list_tail(t, s);
    level--;
    print_level("*RETURN* to expression_list_tail\n");
    
    break;
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "expression list tail");
  }
  return t;
}

Token parse_expression(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_SEMICOLON,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_THEN,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_LPAREN,
    TOKEN_NOT,
    TOKEN_ADDOP,
    TOKEN_BEGIN,
    TOKEN_WHILE,
    TOKEN_LBRACKET
  };
  
  level++;
  
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
    print_level("*RETURN* to expression\n");

    t = parse_expression_tail(t, s);
    level--;
    print_level("*RETURN* to expression\n");

    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "expression");
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
  
  print_level("parse expression_tail\n");
  switch(t.type) {
  case TOKEN_RELOP:

    t = match(TOKEN_RELOP, t, s);
    
    t = parse_simple_expression(t, s);
    level--;
    print_level("*RETURN* to expression_tail\n");

    t = parse_expression_tail(t, s);
    level--;
    print_level("*RETURN* to expression_tail\n");

    break;
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_IF:
  case TOKEN_ELSE:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_RBRACKET:
  case TOKEN_COMMA:
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "expression tail");
  }
  return t;
}

Token parse_simple_expression(Token t, struct state s) {
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
    TOKEN_LPAREN,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_LPAREN,
    TOKEN_NOT,
    TOKEN_ADDOP
  };
  
  level++;
  
  print_level("parse simple_expression\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_term(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    t = parse_simple_expression_tail(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    break;
    
  case TOKEN_ADDOP:
    t = parse_sign(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");
    
    t = parse_term(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");


    t = parse_simple_expression_tail(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    break;
  default:
    printf("got: %s\n", t.str);
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "simple expression");
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
  print_level("parse simple_expression_tail\n");
  
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    t = parse_term(t, s);
    level--;
    print_level("*RETURN* to simple_expression_tail\n");

    t = parse_simple_expression_tail(t, s);
    level--;
    print_level("*RETURN* to simple_expression_tail\n");
    
    break;
  case TOKEN_RELOP:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_IF:
  case TOKEN_ELSE:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_RBRACKET:
  case TOKEN_COMMA:
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "simple expression tail");
  }
  return t;
}

Token parse_term(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_ADDOP,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_NOT,
    TOKEN_LPAREN
  };
  
  level++;
  
  print_level("parse term\n");
  switch(t.type) {
  case TOKEN_ID:
  case TOKEN_INT:
  case TOKEN_REAL:
  case TOKEN_LPAREN:
  case TOKEN_NOT:
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term\n");

    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "term");
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
  
  print_level("parse term_tail\n");
  switch(t.type) {
  case TOKEN_MULOP:
    t = match(TOKEN_MULOP, t, s);
    
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    break;

  case TOKEN_MOD:
    t = match(TOKEN_MOD, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_DIV:
    t = match(TOKEN_DIV, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_AND:
    t = match(TOKEN_AND, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_OR:
    t = match(TOKEN_OR, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;
  case TOKEN_ADDOP:
  case TOKEN_RELOP:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_IF:
  case TOKEN_ELSE:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_RBRACKET:
  case TOKEN_COMMA:
  case TOKEN_RPAREN:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "term tail");
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
  
  print_level("parse factor\n");
  switch(t.type) {
  case TOKEN_ID:
    
    t = match(TOKEN_ID, t, s);

    t = parse_factor_tail(t, s);
    level--;
    print_level("*RETURN* to factor\n");

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
    print_level("*RETURN* to factor\n");
    
    t = match(TOKEN_RPAREN, t, s);
    break;
    
  case TOKEN_NOT:
    t = match(TOKEN_NOT, t, s);

    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to factor\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "factor");
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
  
  print_level("parse factor_tail\n");
  switch(t.type) {

    // factor_tail -> [ expression ]
  case TOKEN_LBRACKET:
    t = match(TOKEN_LBRACKET, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to factor_tail\n");
    t = match(TOKEN_RBRACKET, t, s);
    
    break;

    // factor_tail -> ( expression_list )
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression_list(t, s);
    level--;
    print_level("*RETURN* to factor_tail\n");

    t = match(TOKEN_RPAREN, t, s);
    break;
    
    // factor_tail -> e
  case TOKEN_MULOP:
  case TOKEN_MOD:
  case TOKEN_DIV:
  case TOKEN_AND:
  case TOKEN_ADDOP:
  case TOKEN_RELOP:
  case TOKEN_COMMA:
  case TOKEN_RPAREN:
  case TOKEN_SEMICOLON:
  case TOKEN_END:
  case TOKEN_IF:
  case TOKEN_ELSE:
  case TOKEN_DO:
  case TOKEN_THEN:
  case TOKEN_RBRACKET:
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "factor tail");
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

  print_level("parse sign\n");
  switch(t.type) {
  case TOKEN_ADDOP:
    t = match(TOKEN_ADDOP, t, s);
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "sign");
  }
  return t;
}
