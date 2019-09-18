#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/reserved.h"
Token parse_reserved_word_buffer(char * line);

int parse_reserved_words() {
  char *reserved_word_file = "data/reserved";

  FILE *fp = fopen(reserved_word_file, "r");
  if(fp == NULL) {
    perror("Unable to read reserved word file");
    exit(1);
  }

  printf("reserved word file loaded\n");
  char reserved_word_buffer[30];
  while(fgets(reserved_word_buffer, sizeof reserved_word_buffer, fp) != NULL) {
    Token tok = parse_reserved_word_buffer(reserved_word_buffer);
    fprintf(stdout, "%s ", tok.lexeme);
    fprintf(stdout, "%s ", tok.type);
    fprintf(stdout, "%d ", tok.attr);
    printf("\n");
  }
  
  return 0;
}

Token parse_reserved_word_buffer(char * line) {
  // get first segment in line
  char * token = strtok(line, " ");

  // array to hold segments
  char segments[3][20];
  int i = 0;
  while(token != NULL && i < 3) {
    strcpy(segments[i], token);
    token = strtok(NULL, " ");
    i++;
  }

  // copy segments into Token struct
  Token tok;
  strcpy(tok.lexeme, segments[0]);
  strcpy(tok.type, segments[1]);
  tok.attr = atoi(segments[2]);
  return tok;
}
