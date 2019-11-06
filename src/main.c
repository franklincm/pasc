#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TOKEN
#define TOKEN
#include "headers/token.h"
#endif

void read_print_line(FILE *fp);
void print_symbol_table(node *SymbolTable);

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


    Token t;
    t.str = "";

    while(strcmp(t.str, "EOL") != 0) {
      t = get_token(line_buffer, reserved_words, &symbol_table);
      //printf("%s %d %d\n", t.str, t.type, t.attr);
      char * type = type_to_str(t);
      if(strcmp(type, "TOKEN_WS") != 0) {
        if(t.type == LEXERR) {
          char * attr = attr_to_str(t);
          printf("%s %s\n", type, attr);
        } else {
          printf("%s %d\n", type, t.attr);
        }
      }
    }
  }

  //print_symbol_table(&symbol_table);
  
}

void print_symbol_table(node *SymbolTable) {
  printf("\nSymbol Table\n");
  for(int i = 0; i < 21; i++) {
    putc('=', stdout);
  }
  printf("\n");
  node p = *SymbolTable;
  while (p != NULL) {
    printf("symbol table: %s\n", p->str);
    p = p->next;
  }
}
