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

int offset;
int address;
int width;

int level = 0;
int print = 0;
int EOP = 0;

char buffer [200];

char *subp_head_str;
char *param_id_str;
char *dec_id_str;
char *lhs;
char *format_buffer;
char *profile_buffer;
char *expr_list_profile_buffer;
char *factor_tail_profile_in;
char *var_id;

int statement_compound;
int profile_length;
int insert;

int type;
int standard_type;
int factor_tail_type;
int factor_type;
int expression_type;
int expression_tail_in;
int expression_tail_type;
int simple_expression_type;
int simple_expression_tail_in;
int simple_expression_tail_type;
int term_type;
int term_tail_in;
int term_tail_type;
int expression_list_type;
int expression_list_tail_in;
int expression_list_tail_type;
int variable_tail_in;
int variable_tail_type;
int variable_type;

int semerr_line = 0;
int synerr_line = 0;


void update_profile(int type) {

  char *old_profile = "";
  if (expr_list_profile_buffer)
    old_profile = expr_list_profile_buffer;

  expr_list_profile_buffer = (char *)malloc(sizeof(char) * (strlen(old_profile) + 1));
  sprintf(expr_list_profile_buffer, "%s%d", old_profile, type);
}

void print_semerr(char *msg, FILE *listing) {
  char *sbuffer;
  char *prefix = "SEMERR: ";
  sbuffer = (char *)malloc(sizeof(char) * (strlen(msg) + strlen(prefix)));
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
    printf("SYNERR ");
    fflush(stdout);
    printf("Expecting: %s, Got: %s    %s\n", type_to_str(token_type), type_to_str(t.type), t.str);
    fflush(stdout);
    //write_line_to_file("SYNERR\n", s.listing);


    char * buffer = (char *)malloc(150 * sizeof(char));
    sprintf(buffer, "SYNERR: Expecting: %s, Got: '%s'\n", type_to_str(token_type), t.str);
    write_line_to_file(buffer, s.listing);
    synerr_line = get_lineno();

    // syntax error, skip token
    t = get_tok(s);    
  }
  return t;
}

Token synchronize(Token t, struct state s, int *synch, int size, char *production) {
  char * buffer = (char *)malloc(150 * sizeof(char));
  sprintf(buffer, "SYNERR: Expecting %s, Got: '%s'\n", production, t.str);
  synerr_line = get_lineno();

  if(t.type == TOKEN_EOF) {
    write_line_to_file("End of Parse\n", s.listing);
  }
  
  printf("SYNCH CALLED (%s): %s\n", production, t.str);

  /* for(int token = 0; token < size; token++) { */
  /*   if (t.type == synch[token]) { */
  /*     printf("skipping %s...\n\n", t.str); */

  /*     sprintf(buffer, "SYNCH FOUND: %s\n\n", t.str); */
  /*     write_line_to_file(buffer, s.listing); */
  /*     return t; */
  /*   } */
  /* } */
  
  int synch_found = 0;

  while (!synch_found) {

    for(int token = 0; token < size; token++) {

      if (t.type == synch[token]) {
        synch_found = 1;
        //sprintf(buffer, "skipping...\n\n");
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

  
  struct state s = {source, listing, tokenfile, symboltablefile, reserved_words};
  
  Token t;
  t = get_tok(s);
  parse_program(t, s);
  free(profile_buffer);
  free(format_buffer);
}

Token parse_program(Token t, struct state s) {
  int synch[] = {
    TOKEN_EOF,
    TOKEN_PROGRAM
  };

  level++;
  print_level("parse program\n");

  int err;
  
  switch(t.type) {
  case TOKEN_PROGRAM:
    t = match(TOKEN_PROGRAM, t, s);

    if(t.type != LEXERR) {
      err = check_add_green(t.str, PGNAME, "", address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "Attempt to redefine '%s'.\n", t.str);
        print_semerr(buffer, s.listing);
      } else {
        insert = 1;
      }
    }
    
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
    offset = 0;
    address = 0;
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

  int err;
  
  switch(t.type) {
  case TOKEN_ID:
    if(t.type != LEXERR) {
      err = check_add_blue(t.str, PGPARAM, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", t.str);
        print_semerr(buffer, s.listing);
      }
    }
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

  int err;
  switch(t.type) {
  case TOKEN_COMMA:
    t = match(TOKEN_COMMA, t, s);

    if(t.type != LEXERR) {
      err = check_add_blue(t.str, PGPARAM, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", t.str);
        print_semerr(buffer, s.listing);
      }
    }
    
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

  int err;
  
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

    if(dec_id_str) {
     
      err = check_add_blue(dec_id_str, type, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", dec_id_str);
        print_semerr(buffer, s.listing);
      } else {
        address = address + width;
      }

    }
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

  int err;
  
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

    if(dec_id_str) {

      err = check_add_blue(dec_id_str, type, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", dec_id_str);
        print_semerr(buffer, s.listing);
      } else {
        address = address + width;
      }
    }
    
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
  type = 0;
  int val1, val2;

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

    val1 = atoi(t.str);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_ELIPSIS, t, s);

    val2 = atoi(t.str);
    t = match(TOKEN_INT, t, s);
    t = match(TOKEN_RBRACKET, t, s);
    t = match(TOKEN_OF, t, s);

    t = parse_standard_type(t, s);
    level--;
    print_level("*RETURN* to type\n");

    if (val2 >= val1) {
      width = width * ((val2-val1) + 1);
    } else {
      sprintf(buffer, "invalid array subscript.\n");
      print_semerr(buffer, s.listing);
    }

    type = standard_type + 2;
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
    width = 4;
    break;

  case TOKEN_RREAL:
    t = match(TOKEN_RREAL, t, s);
    standard_type = t_REAL;
    width = 8;
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


    
    if(t.type != LEXERR) {
      subp_head_str = t.str;
    }
    
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

  int err;
  
  switch(t.type) {
  case TOKEN_LPAREN:
    t = parse_arguments(t, s);
    level--;
    print_level("*RETURN* to subprogram_head_tail\n");

    t = match(TOKEN_COLON, t, s);
    t = parse_standard_type(t, s);
    level--;
    print_level("*RETURN* to subprogram_head_tail\n");

    if(subp_head_str) {
      err = check_add_green(subp_head_str, standard_type, profile_buffer, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "Attempt to redefine '%s'.\n", subp_head_str);
        print_semerr(buffer, s.listing);
      } else {
        offset = address + offset;
        address = 0;
        insert = 1;
      }
      subp_head_str = NULL;
    }

    t = match(TOKEN_SEMICOLON, t, s);
    break;

  case TOKEN_COLON:
    t = match(TOKEN_COLON, t, s);

    t = parse_standard_type(t, s);
    level--;
    print_level("*RETURN* to subprogram_head_tail\n");

    if(subp_head_str) {
      err = check_add_green(subp_head_str, standard_type, profile_buffer, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "Attempt to redefine '%s'.\n", t.str);
        print_semerr(buffer, s.listing);
      } else {
        insert = 1;
      }
      subp_head_str = NULL;
    }
    
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

  int err;
  
  switch(t.type) {
  case TOKEN_ID:
    if(t.type != LEXERR)
      param_id_str = t.str;

    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level("*RETURN* to parameter_list\n");
    
    if(param_id_str) {
      err = check_add_blue(param_id_str, type+4, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", param_id_str);
        print_semerr(buffer, s.listing);
      }
      
      // start building profile string
      profile_length = 1;
      profile_buffer = (char *)malloc(sizeof(char) * profile_length);
      sprintf(profile_buffer, "%d", type);
    }
    param_id_str = NULL;
    
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

  int err;
  
  switch(t.type) {
  case TOKEN_SEMICOLON:
    t = match(TOKEN_SEMICOLON, t, s);

    if(t.type != LEXERR)
      param_id_str = t.str;
    
    t = match(TOKEN_ID, t, s);
    t = match(TOKEN_COLON, t, s);
    t = parse_type(t, s);
    level--;
    print_level("*RETURN* to parameter_list_tail\n");

    
    if(param_id_str) {
      err = check_add_blue(param_id_str, type+4, address, offset, s.symboltablefile);
      if (err == 0) {
        sprintf(buffer, "'%s' already defined in this scope.\n", param_id_str);
        print_semerr(buffer, s.listing);
      }
      
      // continue building profile string
      profile_buffer = realloc(profile_buffer, profile_length + 1);
      
      format_buffer = (char *)malloc(sizeof(char) * 1);
      sprintf(format_buffer, "%d", type);

      strcat(profile_buffer, format_buffer);
      profile_length++;
    }
    param_id_str = NULL;

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
    if (!statement_compound && insert) {
      pop_eye();
      insert = 0;
    }

    break;

  case TOKEN_END:
    t = match(TOKEN_END, t, s);
    if (!statement_compound && insert) {
      pop_eye();
      insert = 0;
    }

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

  struct ColorNode *symbol;
  char *st_id;
  
  switch(t.type) {
  case TOKEN_ID:

    st_id = t.str;
    symbol = get_color_node(st_id);

    t = parse_variable(t, s);
    level--;
    print_level("*RETURN* to statement\n");

    t = match(TOKEN_ASSIGN, t, s);
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to statement\n");

    if (symbol) {
      if (symbol->type == t_INT && expression_type == t_INT) {}
      else if (symbol->type == t_PPINT && expression_type == t_INT) {}
      else if (symbol->type == t_INT && expression_type == t_PPINT) {}
      else if (symbol->type == t_PPINT && expression_type == t_PPINT) {}
      else if (symbol->type == t_AINT && expression_type == t_INT) {}
      else if (symbol->type == t_AINT && expression_type == t_PPINT) {}
      else if (symbol->type == t_PPAINT && expression_type == t_INT) {}
      else if (symbol->type == t_PPAINT && expression_type == t_PPINT) {}

      else if (symbol->type == t_REAL && expression_type == t_REAL) {}
      else if (symbol->type == t_PPREAL && expression_type == t_REAL) {}
      else if (symbol->type == t_REAL && expression_type == t_PPREAL) {}
      else if (symbol->type == t_PPREAL && expression_type == t_PPREAL) {}
      else if (symbol->type == t_AREAL && expression_type == t_REAL) {}
      else if (symbol->type == t_AREAL && expression_type == t_PPREAL) {}
      else if (symbol->type == t_PPAREAL && expression_type == t_REAL) {}
      else if (symbol->type == t_PPAREAL && expression_type == t_PPREAL) {}
    
      else {
        sprintf(buffer, "type mismatch, can't assign type '%s' to '%s'\n",
                profile_type_to_str(expression_type),
                profile_type_to_str(symbol->type));
        print_semerr(buffer, s.listing);
      }
    } else {
      sprintf(buffer, "cannot find symbol '%s'.\n", st_id);
    }

    break;
    
  case TOKEN_BEGIN:
    statement_compound = 1;
    t = parse_compound_statement(t, s);
    statement_compound = 0;
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

    if (expression_type == t_SEMERR) {}
    else if (expression_type != t_BOOL) {
      sprintf(buffer, "'while' condition must be type BOOL.\n");
      print_semerr(buffer, s.listing);
    }
    
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

    if (expression_type == t_SEMERR) {}
    else if (expression_type != t_BOOL) {
      sprintf(buffer, "'if' condition must be type BOOL.\n");
      print_semerr(buffer, s.listing);
    }

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

  struct ColorNode *symbol;
  
  switch(t.type) {
  case TOKEN_ID:
    var_id = t.str;
    symbol = get_color_node(var_id);
    t = match(TOKEN_ID, t, s);

    if (symbol)
      variable_tail_in = symbol->type;
    else
      variable_tail_in = t_SEMERR;
    
    t = parse_variable_tail(t, s);
    level--;
    print_level("*RETURN* to variable\n");

    variable_type = variable_tail_type;

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

    // of course there are better ways to do this...
    if (variable_tail_in == t_AINT && expression_type == t_INT) {
      variable_tail_type = t_INT;
    } else if (variable_tail_in == t_AINT && expression_type == t_PPINT) {
      variable_tail_type = t_INT;
    } else if (variable_tail_in == t_PPAINT && expression_type == t_INT) {
      variable_tail_type = t_INT;
    } else if (variable_tail_in == t_PPAINT && expression_type == t_PPINT) {
      variable_tail_type = t_INT;
    }

    else if (variable_tail_in == t_AREAL && expression_type == t_REAL) {
      variable_tail_type = t_REAL;
    } else if (variable_tail_in == t_AREAL&& expression_type == t_PPREAL) {
      variable_tail_type = t_REAL;
    } else if (variable_tail_in == t_PPAREAL&& expression_type == t_REAL) {
      variable_tail_type = t_REAL;
    } else if (variable_tail_in == t_PPAREAL&& expression_type == t_PPREAL) {
      variable_tail_type = t_REAL;
    }

    else if (variable_tail_in == t_SEMERR || expression_type == t_SEMERR) {
      variable_tail_type = t_SEMERR;
    } else if (variable_tail_in != t_AINT && expression_type == t_INT) {
      variable_tail_type = t_SEMERR;
      sprintf(buffer, "'%s' is not an array type and cannot be indexed\n.", var_id);
      print_semerr(buffer, s.listing);
    } else if (variable_tail_in != t_AINT && expression_type == t_PPINT) {
      variable_tail_type = t_SEMERR;
      sprintf(buffer, "'%s' is not an array type and cannot be indexed\n.", var_id);
      print_semerr(buffer, s.listing);
    } else if (variable_tail_in != t_AREAL && expression_type == t_INT) {
      variable_tail_type = t_SEMERR;
      sprintf(buffer, "'%s' is not an array type and cannot be indexed\n.", var_id);
      print_semerr(buffer, s.listing);
    } else if (variable_tail_in != t_AREAL && expression_type == t_PPINT) {
      variable_tail_type = t_SEMERR;
      sprintf(buffer, "'%s' is not an array type and cannot be indexed\n.", var_id);
      print_semerr(buffer, s.listing);
    } else if (expression_type != t_INT) {
      variable_tail_type = t_SEMERR;
      sprintf(buffer, "array indices must be type INT.\n");
      print_semerr(buffer, s.listing);
    }
    
    t = match(TOKEN_RBRACKET, t, s);
    break;
  case TOKEN_ASSIGN:
    variable_tail_type = variable_tail_in;
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

    expr_list_profile_buffer = NULL;
    
    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to expression_list\n");

    update_profile(expression_type);

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

    update_profile(expression_type);
    
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

    expression_tail_in = simple_expression_type;
    
    t = parse_expression_tail(t, s);
    level--;
    print_level("*RETURN* to expression\n");


    if (expression_tail_type == t_PPINT)
      expression_tail_type = t_INT;
    else if (expression_tail_type == t_PPREAL)
      expression_tail_type = t_REAL;
    expression_type = expression_tail_type;
    
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

    if (expression_tail_in == t_INT && simple_expression_type == t_INT) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_INT && simple_expression_type == t_PPINT) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_PPINT && simple_expression_type == t_INT) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_PPINT && simple_expression_type == t_PPINT) {
      expression_tail_in = t_BOOL;
    }

    else if (expression_tail_in == t_REAL && simple_expression_type == t_REAL) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_REAL && simple_expression_type == t_PPREAL) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_PPREAL && simple_expression_type == t_REAL) {
      expression_tail_in = t_BOOL;
    } else if (expression_tail_in == t_PPREAL && simple_expression_type == t_PPREAL) {
      expression_tail_in = t_BOOL;
    }

    else if(expression_tail_in == t_SEMERR || simple_expression_type == t_SEMERR) {
      expression_tail_in = t_SEMERR;
    } else {
      expression_tail_in = t_SEMERR;
      sprintf(buffer, "Illegals comparison. Operands must  be of same type.\n");
      print_semerr(buffer, s.listing);
    }

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
    expression_tail_type = expression_tail_in;
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

    simple_expression_tail_in = term_type;
    
    t = parse_simple_expression_tail(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    simple_expression_type = simple_expression_tail_type;
    
    break;
    
  case TOKEN_ADDOP:
    t = parse_sign(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");
    
    t = parse_term(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    simple_expression_tail_in = term_type;

    t = parse_simple_expression_tail(t, s);
    level--;
    print_level("*RETURN* to simple_expression\n");

    simple_expression_type = simple_expression_tail_type;

    break;
  default:
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

    if (simple_expression_tail_in == t_INT && term_type == t_INT) {
      simple_expression_tail_in = t_INT;
    } else if (simple_expression_tail_in == t_INT && term_type == t_PPINT) {
      simple_expression_tail_in = t_INT;
    } else if (simple_expression_tail_in == t_PPINT && term_type == t_INT) {
      simple_expression_tail_in = t_INT;
    } else if (simple_expression_tail_in == t_PPINT && term_type == t_PPINT) {
      simple_expression_tail_in = t_INT;
    }

    else if (simple_expression_tail_in == t_REAL && term_type == t_REAL) {
      simple_expression_tail_in = t_REAL;
    } else if (simple_expression_tail_in == t_REAL && term_type == t_PPREAL) {
      simple_expression_tail_in = t_REAL;
    } else if (simple_expression_tail_in == t_PPREAL && term_type == t_REAL) {
      simple_expression_tail_in = t_REAL;
    } else if (simple_expression_tail_in == t_PPREAL && term_type == t_PPREAL) {
      simple_expression_tail_in = t_REAL;
    }

    else if (simple_expression_tail_in == t_SEMERR || term_type == t_SEMERR) {
      simple_expression_tail_in = t_SEMERR;
    } else {
      simple_expression_tail_in = t_SEMERR;
      sprintf(buffer, "Mixed mode expressions are not allowed.\n");
      print_semerr(buffer, s.listing);

      sprintf(buffer, "sexp_in = %d, term_type = %d\n", simple_expression_tail_in, term_type);
      print_semerr(buffer, s.listing);
    }

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
    simple_expression_tail_type = simple_expression_tail_in;
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

    term_tail_in = factor_type;

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term\n");

    term_type = term_tail_type;

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
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_RBRACKET
  };
  
  level++;
  
  print_level("parse term_tail\n");
  switch(t.type) {
  case TOKEN_MULOP:
    t = match(TOKEN_MULOP, t, s);
    
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");


    if (term_tail_in == t_INT && factor_type == t_INT) {
      term_tail_in = t_INT;      
    } else if (term_tail_in == t_INT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_INT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    }

    else if (term_tail_in == t_REAL && factor_type == t_REAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_REAL && factor_type == t_PPREAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_PPREAL && factor_type == t_REAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_PPREAL && factor_type == t_PPREAL) {
      term_tail_in = t_REAL;
    }

    else if (term_tail_in == t_SEMERR || factor_type == t_SEMERR) {
      term_tail_in = t_SEMERR;
    } else {
      term_tail_in = t_SEMERR;
      sprintf(buffer, "mulop mixed expressions not allowed\n");
      print_semerr(buffer, s.listing);
    }

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    break;

  case TOKEN_MOD:
    t = match(TOKEN_MOD, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    if (term_tail_in == t_INT && factor_type == t_INT) {
      term_tail_in = t_INT;      
    } else if (term_tail_in == t_INT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_INT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    }

    else if (term_tail_in == t_REAL && factor_type == t_REAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_REAL && factor_type == t_PPREAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_PPREAL && factor_type == t_REAL) {
      term_tail_in = t_REAL;
    } else if (term_tail_in == t_PPREAL && factor_type == t_PPREAL) {
      term_tail_in = t_REAL;
    }

    else if (term_tail_in == t_SEMERR || factor_type == t_SEMERR) {
      term_tail_in = t_SEMERR;
    } else {
      term_tail_in = t_SEMERR;
      sprintf(buffer, "mod mixed expressions not allowed\n");
      print_semerr(buffer, s.listing);
    }

    
    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_DIV:
    t = match(TOKEN_DIV, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    if (term_tail_in == t_INT && factor_type == t_INT) {
      term_tail_in = t_INT;      
    } else if (term_tail_in == t_INT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_INT) {
      term_tail_in = t_INT;
    } else if (term_tail_in == t_PPINT && factor_type == t_PPINT) {
      term_tail_in = t_INT;
    }

    else if (term_tail_in == t_SEMERR || factor_type == t_SEMERR) {
      term_tail_in = t_SEMERR;
    } else {
      term_tail_in = t_SEMERR;
      sprintf(buffer, "div mixed expressions not allowed\n");
      print_semerr(buffer, s.listing);
    }
    

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_AND:
    t = match(TOKEN_AND, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    if (term_tail_in == t_BOOL && factor_type == t_BOOL) {
      term_tail_in = t_BOOL;      
    }

    else if (term_tail_in == t_SEMERR || factor_type == t_SEMERR) {
      term_tail_in = t_SEMERR;
    } else {
      term_tail_in = t_SEMERR;
      sprintf(buffer, "'and' operands must both be type BOOL.\n");
      print_semerr(buffer, s.listing);
    }

    t = parse_term_tail(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");
    break;

  case TOKEN_OR:
    t = match(TOKEN_OR, t, s);
    t = parse_factor(t, s);
    level--;
    print_level("*RETURN* to term_tail\n");

    if (term_tail_in == t_BOOL && factor_type == t_BOOL) {
      term_tail_in = t_BOOL;      
    }

    else if (term_tail_in == t_SEMERR || factor_type == t_SEMERR) {
      term_tail_in = t_SEMERR;
    } else {
      term_tail_in = t_SEMERR;
      sprintf(buffer, "'or' operands must both be type BOOL.\n");
      print_semerr(buffer, s.listing);
    }
    
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
    term_tail_type = term_tail_in;
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

  struct ColorNode *symbol;
  int factor_tail_in;
  char *factor_id;
  char *factor_tail_profile_in;

  switch(t.type) {
  case TOKEN_ID:

    factor_id = t.str;
    symbol = get_color_node(factor_id);

    if(symbol != NULL) {

      factor_tail_profile_in = (char *)malloc(sizeof(char) * strlen(symbol->profile));
      sprintf(factor_tail_profile_in, "%s", symbol->profile);
      factor_tail_in = symbol->type;
    } else {
      factor_tail_in = t_SEMERR;

      factor_tail_profile_in = (char *)malloc(sizeof(char) * 2);
      sprintf(factor_tail_profile_in, "%d", 99);
      
      sprintf(buffer, "Unknown reference to '%s'.\n", factor_id);
      print_semerr(buffer, s.listing);

    }

    t = match(TOKEN_ID, t, s);
    t = parse_factor_tail(t, s, factor_tail_in, factor_id, factor_tail_profile_in);
    level--;
    print_level("*RETURN* to factor\n");

    factor_type = factor_tail_type;
    
    break;
    
  case TOKEN_INT:
    t = match(TOKEN_INT, t, s);
    factor_type = t_INT;
    break;
    
  case TOKEN_REAL:
    t = match(TOKEN_REAL, t, s);
    factor_type = t_REAL;
    break;
    
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);

    t = parse_expression(t, s);
    level--;
    print_level("*RETURN* to factor\n");

    factor_type = expression_type;
    
    t = match(TOKEN_RPAREN, t, s);
    break;
    
  case TOKEN_NOT:
    t = match(TOKEN_NOT, t, s);

    t = parse_factor(t, s);

    if (factor_type == t_SEMERR) {
      factor_type = t_SEMERR;
    } else if (factor_type != t_BOOL) {
      factor_type = t_SEMERR;
      sprintf(buffer, "'not' requires type BOOL.\n");
      print_semerr(buffer, s.listing);
    }
    
    level--;
    print_level("*RETURN* to factor\n");
    break;
  default:
    t = synchronize(t, s, synch, sizeof(synch)/sizeof(synch[0]), "factor");
  }

  return t;
}

Token parse_factor_tail(Token t, struct state s, int factor_tail_in, char *factor_id, char *factor_tail_profile_in) {
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

    if (factor_tail_in == t_AINT && expression_type == t_INT) {
      factor_tail_type = t_INT;
    } else if (factor_tail_in == t_AINT && expression_type == t_PPINT) {
      factor_tail_type = t_INT;
    } else if (factor_tail_in == t_AREAL && expression_type == t_INT) {
      factor_tail_type = t_REAL;
    } else if (factor_tail_in == t_AREAL && expression_type == t_PPINT) {
      factor_tail_type = t_REAL;
    } else if (factor_tail_in == t_SEMERR || expression_type == t_SEMERR) {
      factor_tail_type = t_SEMERR;
    } else {
      factor_tail_type = t_SEMERR;
      sprintf(buffer, "array expressions must be type INT.\n");
      print_semerr(buffer, s.listing);
    }

    t = match(TOKEN_RBRACKET, t, s);
    break;
 
    // factor_tail -> ( expression_list )
  case TOKEN_LPAREN:
    t = match(TOKEN_LPAREN, t, s);
    t = parse_expression_list(t, s);
    level--;
    print_level("*RETURN* to factor_tail\n");

    if (!strcmp(factor_tail_profile_in, expr_list_profile_buffer)) {
      factor_tail_type = factor_tail_in;
    } else {
      factor_tail_type = t_SEMERR;
      sprintf(buffer, "type mismatch in function call '%s'\n", factor_id);
      print_semerr(buffer, s.listing);
    }

    free(factor_tail_profile_in);

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
    factor_tail_type = factor_tail_in;
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
