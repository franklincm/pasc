#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TOKEN
#define TOKEN
#include "headers/token.h"
#endif

void read_print_line(FILE *fp);
char *print_token(Token t, int line_num);
char *print_error(Token t);
void write_symbol_table(node *SymbolTable);

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
  
  FILE *token_file = fopen("tokenfile", "w");
  FILE *listing_file = fopen("listingfile", "w");
  
  char * heading = (char *)malloc(100 * sizeof(char));
  sprintf(heading, "%-9s%-14s%-25s%s\n", "Line No.", "Lexeme", "TOKEN-TYPE", "ATTRIBUTE");
  fprintf(token_file, "%s", heading);

  while(fgets(line_buffer, sizeof line_buffer, fp) != NULL) {

    //fputs(line_buffer, stdout);
    fprintf(listing_file, "%-10d%s", line_num, line_buffer);

    Token t;

    while(strcmp(t.str, "EOL") != 0) {
      t = get_token(line_buffer, reserved_words, &symbol_table);
      char *line = print_token(t, line_num);
      printf("%s", line);
      fprintf(token_file, "%s", line);
      if(t.type == LEXERR) {
        fprintf(listing_file, "%s\n", print_error(t));
      }
    }
    line_num++;
  }
  fclose(listing_file);
  
  if(feof(fp)) {
    printf("         %-14s%-4d%-20s %6d (%s)\n", "EOF", 40, "(EOF)", 0, "NULL");
    fprintf(token_file, "         %-14s%-4d%-20s %6d (%s)\n", "EOF", 40, "(EOF)", 0, "NULL");
  }
  
  fclose(token_file);
  write_symbol_table(&symbol_table);
}

char *print_error(Token t) {
  char *line_buffer = (char *)malloc(150 * sizeof(char));
  sprintf(line_buffer, "%-10s%-30s%s", type_to_str(t), attr_to_str(t), t.str);
  return line_buffer;
}

char *print_token(Token t, int line_num) {
  char *line_buffer = (char *)malloc(150 * sizeof(char));
  
  char * type = (char *)malloc(50 * sizeof(char));
  sprintf(type, "(%s)", type_to_str(t));

  if(t.type != TOKEN_WS) {
    char *attr_str;
    char *attr;
    if (t.type == TOKEN_ID) {
      attr_str = "ptr to sym tab";
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "loc%d", t.attr);
      
    } else if (t.type == TOKEN_INT) {
      attr_str = "int value";
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "%d", t.attr);
      
    } else {
      attr_str = (char *)malloc(3 * sizeof(char));
      sprintf(attr_str, "%s", attr_to_str(t));
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "%d", t.attr);
    }
    sprintf(line_buffer, "%4d     %-14s%-4d%-20s %6s (%s)\n", line_num, t.str, t.type, type, attr, attr_str);
  }
  return line_buffer;
}

void write_symbol_table(node *SymbolTable) {
  FILE *symbol_table_file = fopen("symboltable", "w");
  fprintf(symbol_table_file, "%-10s%s", "location", "id");
  fprintf(symbol_table_file, "\n");
  node p = *SymbolTable;
  int loc = 0;
  while (p != NULL) {
    fprintf(symbol_table_file, "%3s%-5d%2s%s\n", "", loc, "", p->str);
    p = p->next;
    loc++;
  }
  fclose(symbol_table_file);
}
