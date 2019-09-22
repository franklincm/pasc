#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/reserved.h"

node parse_reserved_words() {
  char *reserved_word_file = "data/reserved";

  // open reserved word file for reading
  FILE *fp = fopen(reserved_word_file, "r");
  if(fp == NULL) {
    perror("Unable to read reserved word file");
    exit(1);
  }

  // init empty linked list
  node reserved_words = NULL;
  char line_buffer[30];
  
  // for parsing the 3 "elements" in each line
  char elems[3][20];
  int i;
  while(fgets(line_buffer, sizeof line_buffer, fp) != NULL) {
    // get first element
    char *elem = strtok(line_buffer, " ");
    i = 0;
    //  put elements into elems[][]
    while(elem != NULL && i < 3) {
      strcpy(elems[i], elem);
      elem = strtok(NULL, " ");
      i++;
    }

    // now take the elements in elems,
    // create a node, then insert it
    node n = createNode();
    for(i = 0; i < 3; i++) {

      // allocate mem for char array
      n->str = (char*)malloc((strlen(elems[0]) + 1) *sizeof(char));
      strcpy(n->str, elems[0]);

      // ditto above
      n->type = (char*)malloc((strlen(elems[1]) + 1) *sizeof(char));
      strcpy(n->type, elems[1]);

      // set attr int
      n->attr = atoi(elems[2]);
    }
    reserved_words = insertNode(reserved_words, n);
  }

  // return the first node in the list
  return reserved_words;
}
