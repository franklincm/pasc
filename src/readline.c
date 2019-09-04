#include <stdio.h>
#include <stdlib.h>
#include "headers/relops.h"

int main(void) {
  
  FILE *fp = fopen("data/example.pas", "r");
  if(fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }
  
  char line_buffer[79];
  int i = 1;
  while(fgets(line_buffer, sizeof line_buffer, fp) != NULL) {
    char listing[sizeof line_buffer];
    snprintf(listing, sizeof listing, "%2d: %s", i, line_buffer);
    fputs(listing, stdout);
    i++;
  }

  printf("TESTINT: %d\n", testInt());
  
}
