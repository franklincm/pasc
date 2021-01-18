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
  char *filename = "data/program.pas";
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
  // open symbol table fie
  FILE *symboltablefile = fopen("symboltable", "w");
  
  // get reserved words linked list
  node reserved_words = parse_reserved_words();

  // parse source file
  parse(source, listing, tokenfile, symboltablefile, reserved_words);
  
  fclose(source);
  fclose(listing);
  fclose(tokenfile);
  fclose(symboltablefile);
  
}
