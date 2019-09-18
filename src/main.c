#include <stdio.h>
#include <stdlib.h>
#include "headers/relops.h"
#include "headers/reserved.h"

void read_print_line(FILE *fp);
void parse_line(char line[]);

int main(int argc, char **argv) {

  char *filename = "data/example.pas";
  
  if (argc == 2) {
    filename = argv[1];
  }

  printf("input: %s\n\n", filename);
  
  FILE *fp = fopen(filename, "r");
  if(fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }
  
  read_print_line(fp);
  parse_reserved_words();
}

void read_print_line(FILE *fp) {
  char line_buffer[72];
  int line_num = 1;
  char line_num_str[5];
  while(fgets(line_buffer, sizeof line_buffer, fp) != NULL) {
    snprintf(line_num_str, sizeof line_num_str, "%-2d ", line_num);
    fputs(line_num_str, stdout);
    //fputs(line_buffer, stdout);
    parse_line(line_buffer);
    line_num++;
  }
}


void parse_line(char line[]) {
  for(int i = 0; line[i] != 0; i++) {
    putc(line[i], stdout);
  }
}
