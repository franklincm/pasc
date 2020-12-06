#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/colornode.h"

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

static struct ColorNode *top = NULL;

void push_green(char *lex, int type, char *profile) {
  struct ColorNode *node = malloc(sizeof(struct ColorNode));
  node->lex = lex;
  node->type = type;
  node->profile = profile;
  
  node->next = top;
  top = node;
}

void pop_green() {
  if (top == NULL) {
    return;
  }
  struct ColorNode *tmp;
  tmp = top;
  top = top->next;
  printf("POP Green : %s\n", tmp->lex);
  free(tmp);
}

/*
  returns whether or not the lexeme is found in the existing
  green nodes. 

  0 = not found
  1 = found
 */
int search_green_nodes(char *lex) {
  if (top == NULL) {
    return 0;
  }
  struct ColorNode *tmp;
  tmp = top;
  if (strcmp(tmp->lex, lex) == 0) {
    printf("search: %s\n", tmp->lex);
    printf("found: %s\n", tmp->lex);
    return 1;
  }

  while(tmp->next != NULL) {
    tmp = tmp->next;
    printf("search: %s\n", tmp->lex);
    if (strcmp(tmp->lex, lex) == 0) {
      printf("found: %s\n", tmp->lex);
      return 1;
    }
  }
  return 0;
}

void check_add_green_node(Token t) {
  char *str = t.str;
  int type = t.type;
  char *profile = "test";

  /* TODO: check parent green nodes first */
  if(search_green_nodes(str)) {
    printf("SEMERR: Attempt to redefine `%s`\n", str);
  }

  push_green(str, type, profile);
  printf("PUSH Green : %s\n", str);
}
