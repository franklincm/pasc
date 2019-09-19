#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/reserved.h"

ReservedWord parse_reserved_word_buffer(char * line);

void parse_reserved_words(int n, ReservedWord reserved[n]) {
  char *reserved_word_file = "data/reserved";

  FILE *fp = fopen(reserved_word_file, "r");
  if(fp == NULL) {
    perror("Unable to read reserved word file");
    exit(1);
  }

  char reserved_word_buffer[30];
  int i = 0;
  while(fgets(reserved_word_buffer, sizeof reserved_word_buffer, fp) != NULL) {
    reserved[i] = parse_reserved_word_buffer(reserved_word_buffer);
    i++;
  }
}

ReservedWord parse_reserved_word_buffer(char * line) {
  // get first element in line
  char * elem = strtok(line, " ");

  // array to hold elements
  char elems[3][20];
  int i = 0;
  while(elem != NULL && i < 3) {
    strcpy(elems[i], elem);
    elem = strtok(NULL, " ");
    i++;
  }

  // copy segments into ReservedWord struct
  ReservedWord rw;
  rw.str = (char*)malloc((strlen(elems[0]) + 1) *sizeof(char));
  rw.type = (char*)malloc((strlen(elems[1]) + 1) *sizeof(char));
  
  strcpy(rw.str, elems[0]);
  strcpy(rw.type, elems[1]);
  rw.attr = atoi(elems[2]);
  return rw;
}
