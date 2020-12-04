#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TOKEN
#define TOKEN
#include "headers/token.h"
#endif

#ifndef OUTPUT
#define OUTPUT
#include "headers/output.h"
#endif

#ifndef PARSE
#define PARSE
#include "headers/parse.h"
#endif

void write_symbol_table(node *SymbolTable);

int main(int argc, char **argv) {
  char *filename = "data/example.pas";
  char *listingfilename = "listingfile";
  char *tokenfilename = "tokenfile";

  if (argc == 2) {
    filename = argv[1];
  }

  printf("\ninput: %s\n", filename);
  printf("%s", hr_str(22, '-'));
  // open source file
  FILE *source = fopen(filename, "r");
  if(source == NULL) {
    perror("Unable to open file!");
    exit(1);
  }

  // open listing file
  FILE *listing = fopen(listingfilename, "w");
  // open token file
  FILE *tokenfile = fopen(tokenfilename, "w");

  // get reserved words linked list
  node reserved_words = parse_reserved_words();

  // init symbol table
  static node symbol_table = NULL;

  // parse source file
  parse(source, listing, tokenfile, reserved_words, &symbol_table);

  // write symbol table
  write_symbol_table(&symbol_table);

  fclose(source);
  fclose(listing);
  fclose(tokenfile);
  
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
