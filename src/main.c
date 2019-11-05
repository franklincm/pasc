#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TOKEN
#define TOKEN
#include "headers/token.h"
#endif

void read_print_line(FILE *fp);

int main(int argc, char **argv) {
  char *filename = "data/example.pas";
  
  if (argc == 2) {
    filename = argv[1];
  }

  printf("input: %s\n\n", filename);

  // read source file
  FILE *fp = fopen(filename, "r");
  if(fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }
  
  read_print_line(fp);

}


void read_print_line(FILE *fp) {
  node reserved_words = parse_reserved_words();
  
  // init symbol table
  static node symbol_table = NULL;
  
  char line_buffer[72];
  int line_num = 1;
  char line_num_str[5];
  while(fgets(line_buffer, sizeof line_buffer, fp) != NULL) {

    snprintf(line_num_str, sizeof line_num_str, "%-2d ", line_num);
    fputs(line_num_str, stdout);
    fputs(line_buffer, stdout);
    line_num++;

    //get_token(line_buffer, reserved_words, symbol_table);

    Token t;
    t.str = "";
    t.type = TOKEN_LEXERR;
    //t = get_token(line_buffer, reserved_words, &symbol_table);
    //printf("%s %d %d\n", t.str, t.type, t.attr);
    while(strcmp(t.str, "UNDEF") != 0) {
      t = get_token(line_buffer, reserved_words, &symbol_table);
      //printf("%s %d %d\n", t.str, t.type, t.attr);
      char * type = type_to_str(t);
      if( strcmp(type, "TOKEN_WS") != 0 && strcmp(t.str, "UNDEF") != 0) {
        printf("%s %d\n", type, t.attr);
      }
    }
  }
  
  //fputs(line_buffer, stdout);


  // print symbol table
  printf("\nSymbol Table\n");
  for(int i = 0; i < 21; i++) {
    putc('=', stdout);
  }
  printf("\n");
  node p = symbol_table;
  while (p != NULL) {
    printf("symbol table: %s\n", p->str);
    p = p->next;
  }
}
