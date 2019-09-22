#include <stdio.h>
#include <stdlib.h>
#include "headers/token.h"

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
    //fputs(line_num_str, stdout);
    //fputs(line_buffer, stdout);
    line_num++;

    //get_token(line_buffer, reserved_words, symbol_table);
  }

  fputs(line_buffer, stdout);
  get_token(line_buffer, reserved_words, &symbol_table);
  
  node p = symbol_table;
  while (p != NULL) {
    printf("symbol table: %s\n", p->str);
    p = p->next;
  }
}
